#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h> //定时器
#include <string.h>
#include <fcntl.h>
#include "EV_com.h"
#include "timer.h"
#include "ev_config.h"

static uint8 recvbuf[512],sendbuf[512];
static uint8 snNo = 0;

#define LOG_STYLES_HELLO	( LOG_STYLE_DATETIMEMS | LOG_STYLE_LOGLEVEL | LOG_STYLE_PID | LOG_STYLE_TID | LOG_STYLE_SOURCE | LOG_STYLE_FORMAT | LOG_STYLE_NEWLINE )



static ST_VM_DATA st_vm; 

volatile uint8 pcLock;//PC请求互斥锁
volatile uint8 pcType,pcsubType;//PC请求类型 0表示无请求标志空闲
volatile uint8 pcFlag;//VMC 0空闲 1需要发送请求  2正在处理请求

static uint8 vm_state = EV_STATE_DISCONNECT,last_vm_state = EV_STATE_DISCONNECT;
static Y_FD vmc_fd;

EV_callBack EV_callBack_fun = NULL;


/*********************************************************************************************************
**定时器服务函数 两个定时器 1与VMC通信超时定时器 2PC请求超时定时器
*********************************************************************************************************/
//static int timerId_vmc = 0,timerId_pc = 0;

static ST_TIMER timer_vmc,timer_pc;


//设置PC类型
void EV_set_pc_cmd(const uint8 type)
{ 
	if(type == EV_NA)
	{
        EV_timer_stop(&timer_pc);
		pcFlag = PC_REQ_IDLE;
		pcType = EV_NA;
	}
	else
	{
		pcType = type;
		pcFlag = PC_REQ_SENDING;
	}		
}


uint8	EV_get_pc_cmd()
{
	return pcType;
}



void EV_setSubcmd(uint8 type)
{
	pcsubType = type;

}


uint8 EV_getSubcmd()
{
	return pcsubType;
}

void EV_set_pc_flag(uint8 flag)
{
	pcFlag = flag;
}

uint8 EV_get_pc_flag()
{
	return pcFlag;
}



void EV_setVmState(const uint8 type)
{
	last_vm_state = vm_state; 
	vm_state = type;
	st_vm.state = vm_state;
	st_vm.lastState = last_vm_state;
}


uint8 EV_getVmState()
{
	return vm_state;
}


uint8	EV_getLastVmState()
{
	return last_vm_state;
}


//将VM获取的金额转换为分
uint32 EV_amountFromVM(const uint32 value)
{
    uint32 temp = value * st_vm.setup.vmRatio;
	return temp;
}

//将金额按VM比例 转换
uint32 EV_amountToVM(const uint32 value)
{
    uint32 temp = value / st_vm.setup.vmRatio;
	return temp;
}


/*********************************************************************************************************
** Function name:     pcEncodAmount
** Descriptions:      将32位金额编码成一字节数据
** input parameters:
** output parameters:   无
** Returned value:
*********************************************************************************************************/
uint8 EV_pcEncodAmount(uint32 amount)
{

    unsigned char i = 0,value;
    if(amount == 0)
        return 0;
    while(!(amount % 10))
    {
        amount = amount / 10;
        i++;
    }
    switch(amount)
    {
        case 1:
            value = 1;break;
        case 2:
            value = 2;break;
        case 5:
            value = 5;break;
        default:
            value = 0;break;
    }
    if(value)
    {
        value = (i << 4) | (value & 0x0f);
        return value;
    }
    else
        return 0;

}
/*********************************************************************************************************
** Function name:     pcAnalysisAmount
** Descriptions:      将一字节数据解析为32位金额
** input parameters:
** output parameters:   无
** Returned value:
*********************************************************************************************************/
static uint32 EV_pcAnalysisAmount(uint8 data)
{

    unsigned int amount;
    unsigned char uint;
    if(data == 0)
        return 0;
    uint =  data >> 4;
    amount = data & 0x0f;
    while(uint)
    {
        amount = amount * 10;
        uint--;
    }
    return amount;
}





uint32 EV_vmGetAmount()
{
	return st_vm.remainAmount;
}



void EV_heart_ISR(void)
{
	EV_vmMainFlow(EV_OFFLINE, NULL,0);	
}


void EV_pcTimer_ISR(void)//PC请求超时函数
{
	EV_set_pc_cmd(EV_NA);
	EV_LOGI1("PC request timeout...!!\n");
	if(EV_getVmState() == EV_STATE_INITTING)
	{
		EV_vmMainFlow(EV_OFFLINE, NULL,0);
	}
	else
	{	
		EV_vmMainFlow(EV_TIMEOUT, NULL,0);	
	}
	
}






void EV_callbackhandle(int type,void *ptr)
{
	if(EV_callBack_fun != NULL)
		EV_callBack_fun(type,ptr);
}



void EV_COMLOG(int type ,uint8 *data)
{
	char buf[256] = {0};
    uint16 i;
    for(i = 0;i < (data[1] + 2);i++)
		sprintf(&buf[i*3],"%02x ",data[i]);
	if(type == 1)
		EV_LOGCOM("VM-->PC[%d]:%s\n",data[1],buf);
	else 
		EV_LOGCOM("PC-->VM[%d]:%s\n",data[1],buf);
}




int EV_getCh(char *ch)
{
    return yserial_read(vmc_fd,ch,1);
}




void EV_replyACK(const uint8 flag)
{
    uint8 pc_ack,ix = 0,buf[10] = {0};
    pc_ack = (flag == 1) ? ACK : NAK;
    buf[ix++] = HEAD_EF;
	buf[ix++] = HEAD_LEN;
	buf[ix++] = snNo;
	buf[ix++] = VER_F0_0;
	buf[ix++] = pc_ack;
    uint16 crc = EV_crcCheck(buf,ix);
	buf[ix++] = crc / 256;
	buf[ix++] = crc % 256;
    yserial_write(vmc_fd,(char *)buf,ix);
	EV_COMLOG(2,buf);
}



//0超时 1ACK  2NAK
uint8 EV_recvACK()
{
    uint8 ix = 0,buf[HEAD_LEN + 2],ch;//400ms
    uint16 crc,timeout = 400;//4
	while(timeout)
	{
		if(EV_getCh((char *)&ch))
        {
           buf[ix++] = ch;
           if(ix >= (HEAD_LEN + 2))
		   {
				if(buf[4] == POLL)
				{
					EV_COMLOG(1, buf);
					EV_replyACK(1);ix = 0;
				}	
				else
					break;
		   }    
        }
		else
		{
			EV_msleep(10);
			timeout--;
		}
	}
	if(timeout == 0)
    {
        return 0;
    }
	
	crc = EV_crcCheck(buf,HEAD_LEN);
    if(crc/256 != buf[HEAD_LEN] || crc % 256 != buf[HEAD_LEN + 1])
    {	
		return 0;
    }
	EV_COMLOG(1, buf);
	return buf[4];
}




/*********************************************************************************************************
** Function name	:		EV_sendReq
** Descriptions		:		PC发送串口请求数据包
** input parameters	:   
** output parameters:		无
** Returned value	:		0 失败 1成功  
*********************************************************************************************************/
int EV_sendReq()
{
    uint8 i;
    uint8	len = sendbuf[1];//重定位发送包校验值
    uint8 rAck;
	sendbuf[2] = snNo;
    uint16 crc = EV_crcCheck(sendbuf,len);
	sendbuf[len + 0] = crc / 256;
	sendbuf[len + 1] = crc % 256;	
	for(i = 0;i < 3;i--)
	{
        yserial_write(vmc_fd,(const char *)sendbuf,len + 2);
		EV_COMLOG(2,sendbuf);//输出发送日志

		if(sendbuf[VF] == VER_F0_1) //需要回应ACK
		{
            rAck = EV_recvACK();
			if(rAck == ACK_RPT)//对于短时间的 ACK处理
			{
				EV_set_pc_flag(PC_REQ_HANDLING);
				EV_vmRpt(EV_ACK_VM,NULL,0);
				return 1;
			}
			else if(rAck == NAK_RPT) //vmc拒绝接受命令
			{
				EV_vmRpt(EV_NAK_VM,NULL,0);
				return 1;
			}
		}
		else
		{
			EV_set_pc_cmd(EV_NA);
			return 1;
		}
			
		EV_msleep(500);
		
	}	

	//发送失败
	EV_vmRpt(EV_NAK_VM,NULL,0);
	return 0;
}






/*********************************************************************************************************
** Function name	:		send
** Descriptions		:		PC回应数据包并解析收到的数据包
** input parameters	:   
** output parameters:		无
** Returned value	:		0 失败 1成功  
*********************************************************************************************************/
int EV_send()
{	
    uint8 mt = recvbuf[MT];
	if(snNo == recvbuf[SN])//重包 直接抛弃
    {
		if(recvbuf[VF] == VER_F0_1)
			EV_replyACK(1);
        return 1;
    }
	snNo = recvbuf[SN];
	if((mt == POLL) || (mt == ACTION_RPT && recvbuf[HEAD_LEN] == 5 && recvbuf[HEAD_LEN + 1] == 0x01))
	{
		if(EV_get_pc_flag() == PC_REQ_SENDING)
			EV_sendReq();
		else
		if(recvbuf[3] == VER_F0_1)
			EV_replyACK(1);	
	}
	else 
	{
		if(recvbuf[3] == VER_F0_1)
			EV_replyACK(1);	
	}
	EV_vmRpt(mt,recvbuf,recvbuf[LEN]);

	return 1;


}

int EV_recv()
{
    uint8 ch,ix = 0,len;
    uint16 crc;

    if(!yserial_bytesAvailable(vmc_fd))
	//if(!EV_getCh((char *)&ch))
		return 0;
	EV_getCh((char *)&ch);//HEAD 接受包?
	if(ch != HEAD_EF)
    {
    	//EV_LOGCOM("EV_recv:%02x != %02x\n",ch,HEAD_EF);
        yserial_clear(vmc_fd);
        return 0;
    }
	recvbuf[ix++] = ch;//recvbuf[0]	
	EV_getCh((char *)&ch);//len 接收长度
	if(ch < HEAD_LEN)
    {
        EV_LOGCOM("EV_recv:len = %d < %d\n",ch,HEAD_LEN);
        return 0;
    }
	recvbuf[ix++] = ch;//recvbuf[1]
	len = ch;
	//sn:recvbuf[2] + VER_F:recvbuf[3] + MT:recvbuf[4] + data + crc
	int rcx = 20;
    while(rcx > 0) //200ms
    {
        if(EV_getCh((char *)&ch))
        {
           recvbuf[ix++] = ch;		  
           if(ix >= (len + 2))
               break;
        }
		else
		{
            EV_msleep(10);
			rcx--;
		}
    }
	if(rcx <= 0)
    {
		EV_LOGCOM("EV_recv:timeout!\n");
        return 0;
    }
	
	crc = EV_crcCheck(recvbuf,len);
    if(crc/256 != recvbuf[len] || crc % 256 != recvbuf[len + 1])
    {	
		EV_LOGCOM("EV_recv:crc = Err\n");
		return 0;
    }
	EV_COMLOG(1,recvbuf);
	EV_send();	//接收到 正确的包	
    return 1;
	
}




/*********************************************************************************************************
** Function name:     pcReqSend
** Descriptions:      PC发送数据请求
** input parameters:  type 发送的数据请求类型 不能为零   
** output parameters:   无
** Returned value:    0 失败 1成功  
*********************************************************************************************************/

uint32	EV_pcReqSend(uint8 type,uint8 ackBack,uint8 *data,uint8 len)
{
    uint8	ix = 0;
	int i;
	if(EV_get_pc_flag() != PC_REQ_IDLE)
	{
		EV_callbackhandle(EV_FAIL,"EV_pcReqSend is not PC_REQ_IDLE:\n");
		EV_LOGW("EV_pcReqSend send failed,anther request...");
		return 0;//如果有请求则退出
	}
	sendbuf[ix++] = HEAD_EF;
	sendbuf[ix++] = len + HEAD_LEN;
	sendbuf[ix++] = 0;//预留
    sendbuf[ix++] = ((ackBack == 0) ? VER_F0_0 : VER_F0_1);
	sendbuf[ix++] = type;
	for(i = 0;i < len;i++)
	{
		sendbuf[ix++] = data[i];
	}
	EV_set_pc_cmd(type);
    //EV_LOGTASK("EV_pcReqSend:MT =%x\n",type);
	if(type == VENDOUT_IND)//出货命令 超时1分钟30秒
        EV_timer_start(&timer_pc,EV_TIMEROUT_PC_LONG);
	else						//一般为3秒
        EV_timer_start(&timer_pc,EV_TIMEROUT_PC);
	return 1;

}




/*********************************************************************************************************
** Function name:     	EV_task
** Descriptions:	    PC与VMC通信主任务接口
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void EV_task()
{
	if(EV_recv())
	{
        EV_timer_start(&timer_vmc,EV_TIMEROUT_VMC);
	}
	else
	{
		EV_msleep(50);
	}
		
	
}



static void EV_setup_rpt(ST_SETUP *setup,uint8 *data)
{
    uint8 index = MT + 1;
    uint8 temp = 0,i;
    temp = data[index++];//bin num
    temp = temp;

    setup->language  = data[index++];//language

    setup->payoutTime = data[index++];//pay timeout
	
    setup->vmRatio = data[index++];
    //setup->vmRatio = 10;//目前默认小数点为1

    //特征值 4个字节 前3个字节预留
    index += 3;

    temp = data[index++];
    setup->multBuy = temp & 0x01;
    setup->forceBuy = (temp >> 1) & 0x01;
    setup->humanSensor = (temp >> 2) & 0x01;
    setup->gameButton = (temp >> 3) & 0x01;


    setup->bill.recv_type = data[index++];
    setup->bill.recv_max_value = INTEG32(data[index + 0],data[index + 1],
                        data[index + 2],data[index + 3]);
    index += 4;

    for(i = 0;i < 8;i++)
	{
        setup->bill.recv_ch[i] = EV_pcAnalysisAmount(data[index++]);
	}
    setup->bill.change_type = data[index++];
	for(i = 0;i < 8;i++)
	{
        setup->bill.change_ch[i] = EV_pcAnalysisAmount(data[index++]);
	}


    setup->coin.recv_type = data[index++];
    setup->coin.recv_max_value = INTEG32(data[index + 0],data[index + 1],
            data[index + 2],data[index + 3]);
    index += 4;
	for(i = 0;i < 8;i++)
	{
        setup->coin.recv_ch[i] = EV_pcAnalysisAmount(data[index++]);
	}
    setup->coin.change_type = data[index++];
	for(i = 0;i < 8;i++)
	{
        setup->coin.change_ch[i] = EV_pcAnalysisAmount(data[index++]);
	}

    setup->card.type = data[index++];
    temp = data[index++];
    temp = data[index++];
    temp = data[index++];
    temp = data[index++];
    temp = data[index++];

    temp = data[index++];

    setup->bin.type =  data[index++];




    temp = data[index++];
    setup->bin.addGoods = temp & 0x01;
    setup->bin.goc = (temp >> 1) & 0x01;
    setup->bin.compressor = (temp >> 2) & 0x01;
    setup->bin.light = (temp >> 3) & 0x01;
    setup->bin.hot = (temp >> 4) & 0x01;
    temp = data[index++];
    temp = data[index++];
    temp = data[index++];


    setup->subBin.type =  data[index++];



    temp = data[index++];
    setup->subBin.addGoods = temp & 0x01;
    setup->subBin.goc = (temp >> 1) & 0x01;
    setup->subBin.compressor = (temp >> 2) & 0x01;
    setup->subBin.light = (temp >> 3) & 0x01;
    setup->subBin.hot = (temp >> 4) & 0x01;
    temp = data[index++];
    temp = data[index++];
    temp = data[index++];
	
	
}


static void EV_payin_rpt(uint8 *data)
{
    uint8 index = MT + 1,temp = 0;
    uint32 temp32;
    temp = temp;
	temp = data[index++];//bill or coin
	temp = data[index++];
	temp = data[index++];

    temp32 = INTEG16(data[index + 0],data[index + 1]);
    index += 2;
	st_vm.remainAmount = EV_amountFromVM(temp32);
}


static void EV_trade_rpt(ST_TRADE *trade,uint8 *data)
{
    uint8 index = MT + 1;
    uint32 temp;

    trade->cabinet = data[index++];
    trade->result = data[index++];
    trade->column = data[index++];
    trade->type = data[index++];
    temp = INTEG16(data[index + 0],data[index + 1]);
    index+=2;
    trade->cost = EV_amountFromVM(temp);


    temp = INTEG16(data[index + 0],data[index + 1]);
    index+=2;

    trade->remainAmount = EV_amountFromVM(temp);
    trade->remainCount = data[index++];

}



static void EV_payout_rpt(uint8 *data)
{
    uint8 index = MT + 1,temp = 0;
    uint32 temp32;

    temp = temp;
	temp = data[index++];//bill or coin
	
	temp = data[index++];
	temp = data[index++];



    temp32 = INTEG16(data[index + 0],data[index + 1]);
    index += 2;
	st_vm.remainAmount = EV_amountFromVM(temp32);

	temp = data[index++];
	
}

/*********************************************************************************************************
** Function name	:		EV_vmMainFlow
** Descriptions		:		协议主流程函数
** input parameters	:   
** output parameters:		无
** Returned value	:		无  
*********************************************************************************************************/
int EV_vmMainFlow(const uint8 type,const uint8 *data,const uint8 len)
{
    uint8	buf[256] = {0},temp = len;
	switch(type)
	{
		case EV_NAK_VM:
			if(EV_get_pc_cmd() != EV_NA)//有命令被拒绝
			{
				EV_set_pc_cmd(EV_NA);
				EV_LOGFLOW("EV_NA\n");
				temp = EV_get_pc_cmd();
				EV_callbackhandle(EV_NA,&temp);
			}
			break;
		case EV_TIMEOUT:
			EV_LOGFLOW("EV_TIMEOUT...cmd=%x\n",EV_get_pc_cmd());
			temp = EV_get_pc_cmd();
			EV_callbackhandle(EV_TIMEOUT,&temp);
			break;
        case EV_SETUP_REQ:	//	1 初始化 GET_SETUP PC发送初始化命令
        	if(EV_getVmState() == EV_STATE_DISCONNECT)
        	{
        		EV_set_pc_cmd(EV_NA);
        		EV_setVmState(EV_STATE_INITTING);	
				EV_LOGFLOW("EV_connected,start to init....\n");
				EV_callbackhandle(EV_INITING,recvbuf);
        	}
        	EV_callbackhandle(EV_SETUP_REQ,recvbuf);
			EV_LOGFLOW("EV_SETUP_REQ\n");
			EV_pcReqSend(GET_SETUP,0,NULL,0);
			break;
		case EV_SETUP_RPT://初始化返回信息
            EV_setup_rpt(&st_vm.setup,recvbuf);//vmc_setup包解析
			EV_LOGFLOW("EV_SETUP_RPT\n");
            EV_callbackhandle(EV_SETUP_RPT,&st_vm.setup);
			EV_set_pc_cmd(EV_NA);
			if(EV_getVmState() != EV_STATE_INITTING) //判断是否在初始化
			{
				break;
			}//是初始化 直接下一步	
		
		case EV_CONTROL_REQ://发送初始化完成标志
			EV_LOGFLOW("EV_CONTROL_REQ\n");
			EV_callbackhandle(EV_CONTROL_REQ,recvbuf);
			buf[0] = 19;
			buf[1] = 0x00;
			EV_setSubcmd(19);
			EV_pcReqSend(CONTROL_IND,1,buf,2);
			break;
		case EV_CONTROL_RPT:
			if(EV_get_pc_cmd() == EV_CONTROL_REQ)
				EV_set_pc_cmd(EV_NA);
			if(EV_getSubcmd() == 19)//初始化完成标志 
			{
				if(EV_getVmState() != EV_STATE_INITTING)//是自动初始化跳过直接下一步
				{
					break;
				}
			}
			else
				break;
		case EV_STATE_REQ: // 3.获取售货机状态
			EV_LOGFLOW("EV_STATE_REQ\n");
			EV_callbackhandle(EV_STATE_REQ,recvbuf);
			EV_pcReqSend(GET_STATUS,0,NULL,0);
			break;
		case EV_STATE_RPT://状态返回
			EV_LOGFLOW("EV_STATE_RPT\n");
			if(EV_get_pc_cmd() == EV_STATE_REQ)
			{
				EV_set_pc_cmd(EV_NA);
			}

			if(EV_getVmState() == EV_STATE_INITTING)//初始化状态 表示初始化完毕
			{
				if(data[HEAD_LEN] & 0x03)
					EV_setVmState(EV_STATE_FAULT);
				else
					EV_setVmState(EV_STATE_NORMAL);

                temp = EV_getVmState();
                EV_callbackhandle(EV_STATE_RPT,&temp);
				
			}
			else if(EV_getVmState() == EV_STATE_MANTAIN)//状态是维护 不更改
			{
				temp = EV_getVmState();
				EV_callbackhandle(EV_STATE_RPT,&temp);
				break;
			}
			else
			{
				if(data[HEAD_LEN] & 0x03)
					EV_setVmState(EV_STATE_FAULT);
				else
					EV_setVmState(EV_STATE_NORMAL);
				temp = EV_getVmState();
				EV_callbackhandle(EV_STATE_RPT,&temp);
				break;
			}

		case EV_ONLINE://在线
			EV_callbackhandle(EV_ONLINE,recvbuf);
			EV_LOGFLOW("EV_conneced online....\n");	
			break;
		case EV_OFFLINE://离线
			EV_setVmState(EV_STATE_DISCONNECT);
			EV_LOGFLOW("EV_disconnected......\n");
			EV_callbackhandle(EV_OFFLINE,recvbuf);
			break;
		case EV_ACTION_REQ: // PC动作请求 目前不用
			
			break;
		case EV_ACTION_RPT: //VMC动作报告 
			if(data[MT + 1] == 7)//VMC重启报告
			{
				EV_callbackhandle(EV_RESTART,recvbuf);
				EV_LOGFLOW("VMC is restarted......\n");
				EV_setVmState(EV_STATE_INITTING);
				EV_set_pc_cmd(EV_NA);
				EV_pcReqSend(GET_SETUP,0,NULL,0);
			}
			else if(data[MT + 1] == 5)
			{
				if(data[MT + 2] == 1)//表示进入维护模式
				{
					if(EV_getVmState() != EV_STATE_MANTAIN)
					{
						EV_set_pc_cmd(EV_NA);
						EV_setVmState(EV_STATE_MANTAIN);
						EV_LOGFLOW("EV_ENTER_MANTAIN\n");	
						EV_callbackhandle(EV_ENTER_MANTAIN,recvbuf);
					}
				}
			}
			else
			{
				EV_LOGFLOW("EV_ACTION_RPT\n");	
				EV_callbackhandle(EV_ACTION_RPT,recvbuf);
			}
			break;
		case EV_POLL:
			if(EV_getVmState() == EV_STATE_MANTAIN)//表示退出维护模式
			{
				EV_setVmState(EV_STATE_INITTING);
				EV_LOGFLOW("EV_EXIT_MANTAIN\n");	
				EV_callbackhandle(EV_EXIT_MANTAIN,recvbuf);
				EV_pcReqSend(GET_SETUP,0,NULL,0);
			}
			break;
		case EV_ENTER_MANTAIN:
			
			break;
		case EV_EXIT_MANTAIN:
			break;
		
		case EV_TRADE_RPT: //出货报告返回
			if(EV_get_pc_cmd() == EV_TRADE_REQ)
			{
				EV_set_pc_cmd(EV_NA);			
				EV_LOGFLOW("EV_TRADE_RPT\n");
                EV_trade_rpt(&st_vm.trade,recvbuf);
                EV_callbackhandle(EV_TRADE_RPT,&st_vm.trade);
			}
			
			break;


		case EV_PAYIN_RPT://投币上报
			EV_LOGFLOW("EV_PAYIN_RPT\n");
			EV_payin_rpt(recvbuf);
			EV_callbackhandle(EV_PAYIN_RPT,&st_vm);
			break;
		case EV_PAYOUT_RPT:
			EV_payout_rpt(recvbuf);
			if(EV_get_pc_cmd() == EV_PAYOUT_REQ || 
				(EV_get_pc_cmd() == EV_CONTROL_REQ && EV_getSubcmd() == 6))
			{
				EV_set_pc_cmd(EV_NA);
				EV_LOGFLOW("EV_PAYOUT_RPT\n");
				EV_callbackhandle(EV_PAYOUT_RPT,&st_vm);
			}
			
			break;
		
		case EV_BUTTON_RPT://按键上报
			EV_LOGFLOW("EV_BUTTON_RPT\n");
			if(recvbuf[MT + 1] == 0)//游戏按键
			{
				buf[0] = 0;
				buf[1] = 1;
				buf[2] = recvbuf[MT + 2];
			}
			else if(recvbuf[MT + 1] == 1)//货道按键
			{
				buf[0] = 1;
				buf[1] = recvbuf[MT + 2];
				buf[2] = recvbuf[MT + 3];
			}
			else if(recvbuf[MT + 1] == 2)//退币按键
			{
				buf[0] = 2;
				buf[1] = 1;
				buf[2] = recvbuf[MT + 2];
			}
			
			EV_callbackhandle(EV_BUTTON_RPT,buf);
			break;
		
		default:
			break;
	}
	return 1;

}



/*********************************************************************************************************
** Function name	:		EV_vmcRpt
** Descriptions		:		VMC回应数据包接口 所有回应的数据均需通过此接口过滤
** input parameters	:   	type :MT 包类型
** output parameters:		无
** Returned value	:		无  
*********************************************************************************************************/
int	EV_vmRpt(const uint8 type,const uint8 *data,const uint8 len)
{
    uint8 ev_type = type;

	if(type == EV_POLL)
	{
		if(EV_getVmState() == EV_STATE_DISCONNECT)//如果断线了则自动进入初始化流程
		{
			EV_vmMainFlow(EV_SETUP_REQ,NULL,0);
			return 1;
		}
	}
	else if(type == EV_ACK_VM)
	{
		if(EV_get_pc_cmd() == EV_CONTROL_REQ)
		{	
            if(EV_getSubcmd() != 6)//不是找零
            {
				ev_type = EV_CONTROL_RPT;
				EV_vmMainFlow(ev_type,data,len);
				return 1;
			}

			
		}
	}

	EV_vmMainFlow(type,data,len);
	return 1;

}
	



int EV_pcTrade(uint8 cabinet,uint8 column,uint8 type,uint32 cost)
{
    uint8 buf[20],ix = 0;
    uint32 temp;
	buf[ix++] = cabinet & 0xFF;//pReq->cabinet & 0xFF;
	buf[ix++] = 2;//pReq->type  & 0xFF;
	buf[ix++] = column  & 0xFF;//pReq->id  & 0xFF;
	buf[ix++] = type  & 0xFF;//pReq->payMode  & 0xFF;

	temp = EV_amountToVM(cost);
	buf[ix++] = HUINT16(temp);//pReq->cost / 256;
	buf[ix++] = LUINT16(temp);//pReq->cost % 256;

    return EV_pcReqSend(VENDOUT_IND,1,buf,ix);
}


int EV_pcPayout(int value)
{
    uint8 buf[20],ix = 0;
    uint32 temp = EV_amountToVM(value);
    EV_LOGI("temp= %d\n",temp);
	buf[ix++] = 6;//找零指令
    buf[ix++] = 0;
    buf[ix++] = 0;
	EV_setSubcmd(6);
	return EV_pcReqSend(EV_CONTROL_REQ,1,buf,ix);
}



//直接返回 不用回调
int32 EV_cash_control(uint8 flag)
{
    uint8 buf[20],ix = 0;
    buf[ix++] = 2;
    buf[ix++] = (flag == 0) ? 0 : 1;
    EV_setSubcmd(2);
    return EV_pcReqSend(EV_CONTROL_REQ,1,buf,ix);
}


int32 EV_cabinet_control(uint8 cabinet,uint8 dev,uint8 flag)
{
    uint8 buf[20],ix = 0;
    buf[ix++] = 3;
    buf[ix++] = cabinet;
    buf[ix++] = dev;
    buf[ix++] = (flag == 0) ? 0 : 1;
    EV_setSubcmd(3);
    return EV_pcReqSend(EV_CONTROL_REQ,1,buf,ix);
}

int32 EV_set_date(ST_DATE *date)
{
    uint8 buf[20],ix = 0;
    buf[ix++] = 17;
    buf[ix++] = date->year / 256;
    buf[ix++] = date->year % 256;
    buf[ix++] = date->moth;
    buf[ix++] = date->day;
    buf[ix++] = date->hour;
    buf[ix++] = date->min;
    buf[ix++] = date->sec;
    buf[ix++] = date->week;

    EV_setSubcmd(17);
    return EV_pcReqSend(EV_CONTROL_REQ,1,buf,ix);
}





int EV_openSerialPort(char *portName,int baud,int databits,char parity,int stopbits)
{
    Y_FD fd = yserial_open(portName);
    if (yserial_fdIsNull(fd)){
			EV_LOGE("EV_openSerialPort:oepn %s failed\n",portName);
			return -1;
	}
    yserial_setParity(fd,parity);
    yserial_setBaudRate(fd,baud);
    yserial_setDataBits(fd,databits);
    yserial_setStopBits(fd,stopbits);
    yserial_setTimeout(fd,10);
    yserial_clear(fd);
	vmc_fd = fd;

    EV_LOGI4("EV_openSerialPort:Serial[%s] open suc\n",portName);
    return 1;
}



int EV_closeSerialPort()
{
    yserial_close(vmc_fd);
    EV_LOGI4("EV_closeSerialPort:closed...\n");
    return 1;
}



int EV_register(EV_callBack callBack)
{
    int ret;
	if(callBack == NULL)
	{
		EV_LOGW("The callback is NULL.....\n");
	}
	EV_callBack_fun = callBack;

    timer_vmc.isr = (EV_timerISR)EV_heart_ISR;
    ret = EV_timer_register(&timer_vmc);
    if(ret < 0)
	{
        EV_LOGE("Timer[timerId_vmc] register failed.....\n");
		return -1;
	}
		
    timer_pc.isr = (EV_timerISR)EV_pcTimer_ISR;
    ret = EV_timer_register(&timer_pc);
    if(ret < 0)
	{
        EV_LOGE("Timer[timerId_pc] register failed.....\n");
		return -1;
	}	
	EV_setVmState(EV_STATE_DISCONNECT);
    st_vm.setup.vmRatio = 10;


    EV_LOGI7("EV_register OK.....\n");



	
#if 0
	//创建文件
	log_fd = CreateLogHandle() ;
	if( log_fd == NULL )
	{
		EV_callBack_fun(3,"CreateLogHandle fail/////////////////////");
		return -1;
	}
	EV_callBack_fun(3,"CreateLogHandle:suc111111111111111111111111111111111");
	SetLogOutput(log_fd, LOG_OUTPUT_FILE , "./ev_test.log" , LOG_NO_OUTPUTFUNC );
	SetLogLevel( log_fd , LOG_LEVEL_INFO );
	SetLogStyles( log_fd , LOG_STYLES_HELLO , LOG_NO_STYLEFUNC );

	DebugLog( log_fd , __FILE__ , __LINE__ , "hello DEBUG" );
	InfoLog( log_fd , __FILE__ , __LINE__ , "hello INFO" );
	WarnLog( log_fd , __FILE__ , __LINE__ , "hello WARN" );
	ErrorLog( log_fd , __FILE__ , __LINE__ , "hello ERROR" );
	FatalLog( log_fd , __FILE__ , __LINE__ , "hello FATAL" );
	DestroyLogHandle( log_fd );
#endif	
	
	return 1;
}

int EV_release()
{
    EV_timer_release(&timer_vmc);
    EV_timer_release(&timer_pc);
    EV_closeSerialPort();
	EV_callBack_fun = NULL;
	EV_LOGI7("EV_release OK.....");
	return 1;
}




#ifndef _EV_COM_H_
#define _EV_COM_H_

#include "yoc_serialport.h"


#define HEAD_EF     0xE5
#define HEAD_LEN    5
#define VER_F0_1    0x41
#define VER_F0_0    0x40


#define HEAD   0
#define LEN    1
#define SN     2
#define VF     3
#define MT     4

//VMC-->PC
#define ACK_RPT    0x01
#define NAK_RPT    0x02
#define POLL       0x03
#define VMC_SETUP  0x05
#define PAYIN_RPT 0x06
#define PAYOUT_RPT 0x07
#define HUODAO_RPT 0x0E
#define VENDOUT_RPT 0x08
#define INFO_RPT 0x11
#define ACTION_RPT 0x0B
#define BUTTON_RPT  0x0C
#define STATUS_RPT  0x0D

//PC-->VMC
#define ACK      0x80
#define NAK      0x81
#define GET_SETUP   0x90
#define GET_HUODAO   0x8A
#define HUODAO_IND  0x87
#define SALEPRICE_IND 0x8E
#define HUODAO_SET_IND 0x8F
#define PRICE_SET_IND  0x8E
#define VENDOUT_IND   0x83
#define CONTROL_IND   0x85
#define GET_INFO 0x8C
#define GET_INFO_EXP 0x92
#define SET_HUODAO  0x93
#define GET_STATUS  0x86
#define PAYOUT_IND	0x89

//VMC当前状态
#define EV_STATE_DISCONNECT		0    //断开连接
#define EV_STATE_INITTING		1    //正在初始化
#define EV_STATE_NORMAL			2    //正常
#define EV_STATE_FAULT			3    //故障
#define EV_STATE_MANTAIN		4    //维护


#define PC_REQ_IDLE		0
#define PC_REQ_SENDING	1
#define PC_REQ_HANDLING	2



#define EV_NA               0x00
#define EV_SETUP_REQ        0x90 //初始化请求
#define EV_SETUP_RPT 		0x05//初始化结果返回
#define EV_INFO_REQ 		0x8C
#define EV_INFO_RPT 		0x11
#define EV_ACK_PC           0x80

#define EV_NAK_PC           0x81//PC回应NAK
#define EV_ACK_VM           0x01
#define EV_NAK_VM           0x02
#define EV_POLL             0x03
#define EV_PAYIN_RPT        0x06//投币报告
#define EV_COLUMN_REQ       0x8A//获取货道
#define EV_COLUMN_RPT       0x0E//货道报告
#define EV_TRADE_REQ        0x83
#define EV_TRADE_RPT        0x08

#define EV_ACTION_RPT       0x0B
#define EV_STATE_REQ        0x86
#define EV_STATE_RPT        0x0D
#define EV_BUTTON_RPT       0x0C
#define EV_CONTROL_REQ      0x85
#define EV_PAYOUT_REQ       0x89
#define EV_PAYOUT_RPT       0x07
#define EV_CONTROL_RPT      0xA0
#define EV_ACTION_REQ       0xA1
#define EV_ENTER_MANTAIN    0xA2
#define EV_EXIT_MANTAIN     0xA3

#define EV_INITING          0xA4
#define EV_RESTART          0xA5
#define EV_OFFLINE          0xA6
#define EV_ONLINE           0xA7
#define EV_TIMEOUT          0xA8
#define EV_FAIL             0xA9
#define EV_EXIT_MANTAIN     0xA3



#define EV_TIMEROUT_VMC  10  //10秒超时
#define EV_TIMEROUT_PC   10  //30秒超时
#define EV_TIMEROUT_PC_LONG   90  //30秒超时

typedef struct _st_bill_{
    quint8      recvType;
    quint32     maxRecv;
    quint32     recvChannel[8];
    quint8      changeType;
    quint32     changeChannel[8];


}ST_BILL;


typedef struct _st_coin_{
    quint8 recvType;
    quint32  maxRecv;
    quint32  recvChannel[8];
    quint8 changeType;
    quint32  changeChannel[8];


}ST_COIN;


typedef struct _st_card_{
    quint8 type;
}ST_CARD;


typedef struct _st_bin_{
    quint8 type;//0关闭 1弹簧 2老式升降机 3升降机+传送带 4升降机+弹簧
    quint8 addGoods;//补货方式 0手动 1自动
    quint8 goc;//0关闭出货确认  1开启
    quint8 light;//0不支持 1支持
    quint8 hot;//
    quint8 cool;
    quint8 keyboard;
    quint8 compressor;
}ST_BIN;


typedef struct _st_trade_{
    quint8 cabinet;
    quint8 column;
    quint8 result;
    quint8 type;
    quint8 cost;
    quint32 remainAmount;
    quint8 remainCount;
}ST_TRADE;


typedef struct _st_state_{
    quint8 state;//VMC当前状态
    quint8 bill;
    quint8 coin;
    quint8 cabinet;
    quint8 billch[8];//纸币找零量
    quint8 coinch[8];//硬币找零量

}ST_STATE;


typedef struct _st_setup_{
    quint8  language;
    quint32 vmRatio;//VMC主控板比例因子 1表示分 10表示角  100表示元
    quint8  payoutTime;//超时退币  单位秒  255 表示不退币
    quint8  multBuy;//多次购买
    quint8  forceBuy;//强制购买
    quint8  humanSensor;//是否支持人体接近感应
    quint8  gameButton;
    ST_BILL bill;
    ST_COIN coin;
    ST_CARD card;
    ST_BIN	bin;
    ST_BIN  subBin;

}ST_SETUP;

typedef struct _st_vm_data_{
    ST_SETUP setup;
    ST_TRADE trade;
    //实时状态
    quint8 state;
    quint8 lastState;
    quint32 remainAmount;//当前投币余额


}ST_VM_DATA;


typedef struct _st_date_{
    quint16 year;
    quint8 moth;
    quint8 day;
    quint8 hour;
    quint8 min;
    quint8 sec;
    quint8 week;
}ST_DATE;



typedef void (*EV_callBack)(const int,const void *);
int EV_closeSerialPort();
int EV_openSerialPort(char *portName,int baud,int databits,char parity,int stopbits);
int EV_register(EV_callBack callBack);
int EV_release();
int EV_vmMainFlow(const quint8 type,const quint8 *data,const quint8 len);
int EV_vmRpt(const quint8 type,const quint8 *data,const quint8 len);

quint8 EV_getVmState();
void EV_task();
quint32 EV_vmGetAmount();
int EV_pcTrade(quint8 cabinet,quint8 column,quint8 type,quint32 cost);
int EV_pcPayout(quint32 value);
qint32 EV_set_date(ST_DATE *date);
qint32 EV_cabinet_control(quint8 cabinet,quint8 dev,quint8 flag);
qint32 EV_cash_control(quint8 flag);


quint32	EV_pcReqSend(quint8 type,quint8 ackBack,quint8 *data,quint8 len);

quint32 EV_amountFromVM(const quint32 value);
quint32 EV_amountToVM(const quint32 value);
#endif

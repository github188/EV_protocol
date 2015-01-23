#ifndef _EV_COM_H_
#define _EV_COM_H_

#include "yoc_serialport.h"
#include "EVprotocol.h"

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



#define EV_TIMEROUT_VMC  10  //10秒超时
#define EV_TIMEROUT_PC   10  //30秒超时
#define EV_TIMEROUT_PC_LONG   90  //30秒超时



typedef struct _st_vm_data_{
    ST_SETUP setup;
    ST_TRADE trade;
    //实时状态
    uint8 state;
    uint8 lastState;
    uint32 remainAmount;//当前投币余额


}ST_VM_DATA;


typedef struct _st_date_{
    uint16 year;
    uint8 moth;
    uint8 day;
    uint8 hour;
    uint8 min;
    uint8 sec;
    uint8 week;
}ST_DATE;



typedef void (*EV_callBack)(const int,const void *);
int EV_closeSerialPort();
int EV_openSerialPort(char *portName,int baud,int databits,char parity,int stopbits);
int EV_register(EV_callBack callBack);
int EV_release();
int EV_vmMainFlow(const uint8 type,const uint8 *data,const uint8 len);
int EV_vmRpt(const uint8 type,const uint8 *data,const uint8 len);

uint8 EV_getVmState();
void EV_task();
uint32 EV_vmGetAmount();
int EV_pcTrade(uint8 cabinet,uint8 column,uint8 type,uint32 cost);
int EV_pcPayout(int value);
uint32	EV_pcRequest(uint8 type,uint8 ackBack,uint8 *data,uint8 len);
int32 EV_set_date(ST_DATE *date);
int32 EV_cabinet_control(uint8 cabinet,uint8 dev,uint8 flag);
int32 EV_cash_control(uint8 flag);
uint32	EV_pcReqSend(uint8 type,uint8 ackBack,uint8 *data,uint8 len);
uint32 EV_amountFromVM(const uint32 value);
uint32 EV_amountToVM(const uint32 value);
#endif

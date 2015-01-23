#ifndef _YOC_SERIALPORT_H_
#define _YOC_SERIALPORT_H_

#ifdef EV_UNIX
#include "unix_yocSerialPort.h"
#else
#include "win_yocSerialPort.h"
#endif




#ifdef EV_UNIX
#define  Y_FD  int
#else
#define  Y_FD  HANDLE
#endif



Y_FD yserial_open(char *portName);
void yserial_close(Y_FD fd);
uint32 yserial_read (Y_FD fd,char *pData,uint32 len);
uint32 yserial_write (Y_FD fd,const char* pData, uint32 len);
int yserial_setRWBuffer(Y_FD fd,uint32 dwInQueue,uint32 dwOutQueue);
uint32 yserial_bytesAvailable(Y_FD fd);
void yserial_setBaudRate(Y_FD fd, BaudRateType baudRate);
void yserial_setDataBits(Y_FD fd, DataBitsType dataBits);
void yserial_setStopBits(Y_FD fd,StopBitsType stopBits);
void yserial_setParity(Y_FD fd,ParityType parity);
void yserial_setFlowControl(Y_FD fd,FlowType flow);
void yserial_setTimeout(Y_FD fd,long millisec);
void yserial_clear(Y_FD fd);
uint8 yserial_fdIsNull(const Y_FD fd);



#endif

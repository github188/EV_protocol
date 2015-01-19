#ifndef _YOC_SERIALPORT_H_
#define _YOC_SERIALPORT_H_
#ifdef _TTY_POSIX_
#include "unix_yocSerialPort.h"
#else
#include "win_yocSerialPort.h"
#endif


#ifdef _WIN32
#define OS_IS_WIN32
#else
#endif

#ifdef OS_IS_WIN32
#define  Y_FD  HANDLE
#else
#define  Y_FD  int
#endif



Y_FD yserial_open(char *portName);
void yserial_close(Y_FD fd);
quint32 yserial_read (Y_FD fd,char *pData,quint32 len);
quint32 yserial_write (Y_FD fd,const char* pData, quint32 len);
int yserial_setRWBuffer(Y_FD fd,quint32 dwInQueue,quint32 dwOutQueue);
quint32 yserial_bytesAvailable(Y_FD fd);
void yserial_setBaudRate(Y_FD fd, BaudRateType baudRate);
void yserial_setDataBits(Y_FD fd, DataBitsType dataBits);
void yserial_setStopBits(Y_FD fd,StopBitsType stopBits);
void yserial_setParity(Y_FD fd,ParityType parity);
void yserial_setFlowControl(Y_FD fd,FlowType flow);
void yserial_setTimeout(Y_FD fd,long millisec);
void yserial_clear(Y_FD fd);
quint8 yserial_fdIsNull(const Y_FD fd);



#endif

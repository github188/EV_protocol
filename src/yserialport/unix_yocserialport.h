#ifndef _WIN_YOCSERIALPORT_H_
#define _WIN_YOCSERIALPORT_H_
#include "yoc_serialbase.h"
#include <stdio.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/select.h>




int unixserial_open(char *portName);
void unixserial_setBaudRate(int fd, BaudRateType baudRate);
int unixserial_setTimeout(int fd,long millisec);
int unixserial_setFlowControl(int fd,FlowType flow);
int unixserial_setStopBits(int fd,StopBitsType stopBits);
int unixserial_setParity(int fd,ParityType parity);
int unixserial_setDataBits(int fd,int databits);
quint32 unixserial_bytesAvailable(int fd);
void unixserial_close(int fd);
quint32 unixserial_read (int fd,char *pData,quint32 len);
quint32 unixserial_write (int fd,const char* pData, quint32 len);
void unixserial_clear(int fd);


#endif

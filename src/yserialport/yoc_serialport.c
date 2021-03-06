#include "yoc_serialport.h"
#include "ev_config.h"



uint8 yserial_fdIsNull(const Y_FD fd)
{
#ifdef EV_WIN32
    return (fd == NULL);
#else
    return (fd < 0);
#endif
}

void yserial_close(Y_FD fd)
{
#ifdef EV_WIN32
    winserial_close(fd);
#else  //linux待完善
    unixserial_close(fd);
    EV_LOGD("yserial_close:fd = %d\n",fd);
#endif
}


Y_FD yserial_open(char *portName)
{
    Y_FD fd;
#ifdef EV_WIN32
    fd  = winserial_open(portName);
    winserial_setRWBuffer(fd,1024,1024);
    winserial_setBaudRate(fd,BAUD9600);
    winserial_setDataBits(fd,DATA_8);
    winserial_setStopBits(fd,STOP_1);
    winserial_setParity(fd,PAR_NONE);
    winserial_setFlowControl(fd,FLOW_OFF);
    winserial_setTimeout(fd,10);

    return fd;
#else
    EV_LOGD("yserial_open:[%s]\n",portName);
    fd = unixserial_open(portName);
    unixserial_setBaudRate(fd,BAUD9600);
    unixserial_setDataBits(fd,DATA_8);
    unixserial_setStopBits(fd,STOP_1);
    unixserial_setParity(fd,PAR_NONE);
    unixserial_setFlowControl(fd,FLOW_OFF);
    unixserial_setTimeout(fd,10);
    EV_LOGD("yserial_open:fd = %d\n",fd);
    return fd;
#endif
}



uint32 yserial_read (Y_FD fd,char *pData,uint32 len)
{
#ifdef EV_WIN32
    return winserial_read(fd,pData,len);
#else
    return unixserial_read(fd,pData,len);
#endif
}


uint32 yserial_write (Y_FD fd,const char* pData, uint32 len)
{
#ifdef EV_WIN32
    return winserial_write(fd,pData,len);
#else
    return unixserial_write(fd,pData,len);
#endif
}


int yserial_setRWBuffer(Y_FD fd,uint32 dwInQueue,uint32 dwOutQueue)
{
#ifdef EV_WIN32
    return winserial_setRWBuffer(fd,(DWORD)dwInQueue,(DWORD)dwOutQueue);
#else
    return 1;
#endif
}



uint32 yserial_bytesAvailable(Y_FD fd)
{
#ifdef EV_WIN32
    return winserial_bytesAvailable(fd);
#else
    return unixserial_bytesAvailable(fd);
#endif
}


void yserial_setBaudRate(Y_FD fd, BaudRateType baudRate)
{
#ifdef EV_WIN32
    winserial_setBaudRate(fd,baudRate);
#else
    unixserial_setBaudRate(fd,baudRate);
#endif
}



void yserial_setDataBits(Y_FD fd, DataBitsType dataBits)
{
#ifdef EV_WIN32
    winserial_setDataBits(fd,dataBits);
#else
    unixserial_setDataBits(fd,dataBits);
#endif
}


void yserial_setStopBits(Y_FD fd,StopBitsType stopBits)
{
#ifdef EV_WIN32
    winserial_setStopBits(fd,stopBits);
#else
    unixserial_setStopBits(fd,stopBits);
#endif
}



void yserial_setParity(Y_FD fd,ParityType parity)
{
#ifdef EV_WIN32
    winserial_setParity(fd,parity);
#else
    unixserial_setParity(fd,parity);
#endif
}



void yserial_setFlowControl(Y_FD fd,FlowType flow)
{
#ifdef EV_WIN32
    winserial_setFlowControl(fd,flow);
#else
    unixserial_setFlowControl(fd,flow);
#endif
}



void yserial_setTimeout(Y_FD fd,long millisec)
{
#ifdef EV_WIN32
    winserial_setTimeout(fd,millisec);
#else
    unixserial_setTimeout(fd,millisec);
#endif




}



void yserial_clear(Y_FD fd)
{
#ifdef EV_WIN32
    winserial_clear(fd);
#else
    unixserial_clear(fd);
#endif
}

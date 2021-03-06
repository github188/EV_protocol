#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<pthread.h>
#include "EV_com.h"
#include "EV_bento.h"
#include "ev_config.h"
#include "timer.h"
#include "EVprotocol.h"


static pthread_t pid;
static volatile int g_threadStop = 0;

static EV_CALLBACK_HANDLE EV_callBack_handle = NULL;


static void JNI_callBack(const int type,const void *ptr)
{
    if(EV_callBack_handle)
        EV_callBack_handle(type,ptr);
}

static void *EV_run(void* arg)
{
    EV_register(JNI_callBack);//注册回调
    EV_LOGI("EV_Thread_fun ready...\n");
    while(!g_threadStop)
    {
       EV_task();
    }
    EV_release();
    EV_callBack_handle = NULL;
    EV_LOGI("Thread stopped....");
    pid = 0;
    pthread_exit(0);

    return NULL;
}





int EV_API EV_vmcStart(char *portName,EV_CALLBACK_HANDLE callBack)
{
    void *ret;
    int rst;
    EV_createLog();
    EV_callBack_handle = callBack;
    EV_closeSerialPort();
    //串口打开成功  开启线程
    if(pid)//线程已经开启了  关闭线程
    {
        EV_LOGI("The serialport thread has runing!!!!");
        g_threadStop = 1;
        pthread_join(pid,&ret);
    }

    int fd = EV_openSerialPort(portName,9600,8,'N',1);
    if (fd < 0){
            EV_LOGE("Can't Open Serial Port:%s!",portName);
            return -1;
    }
    EV_LOGI("EV_openSerialPort suc.....!\n");

    g_threadStop = 0;
    rst = pthread_create(&pid, NULL, EV_run, NULL);
    if(rst == 0)
        return 1;
    else
        return -1;
}




void EV_API EV_vmcStop()
{
    EV_LOGI("EV_vmcStop\n");
    g_threadStop = 1;
}



int EV_API EV_trade(int cabinet,int column,int type,long cost)
{
    return EV_pcTrade(cabinet & 0xFF,column & 0xFF,type & 0xFF,cost);
}




int EV_API EV_payout(long value)
{
    return  EV_pcPayout(value);

}

int EV_API EV_payback()
{
    return EV_pcPayback();
}


int EV_API EV_getColumn(int cabinet)
{
    return EV_get_column(cabinet);
}

int EV_API EV_getStatus()
{
    return EV_pcRequest(GET_STATUS,0,NULL,0);
}


long EV_API EV_getRemainAmount()
{
    return EV_vmGetAmount();
}




int EV_API  EV_bentoRegister(char *portName)
{
    EV_bento_closeSerial();
    int fd = EV_bento_openSerial(portName,9600,8,'N',1);
    if (fd < 0){
            EV_LOGE("Can't Open Serial Port:%s!",portName);
            return -1;
    }
    return 1;
}


int EV_API EV_bentoOpen(int cabinet, int box)
{
    return EV_bento_open(cabinet,box);
}

int EV_API EV_bentoRelease()
{
   return EV_bento_closeSerial();
}


int EV_API EV_bentoLight(int cabinet, int flag)
{
    return EV_bento_light(cabinet,flag);
}


int EV_API EV_bentoCheck(int cabinet,ST_COLUMN_RPT *info)
{
    return EV_bento_check(cabinet,info);
}




int EV_API EV_cashControl(int flag)
{
    return EV_cash_control(flag);
}



int EV_API EV_cabinetControl(int cabinet, int dev, int flag)
{
    return EV_cabinet_control(cabinet,dev,flag);
}


int EV_API EV_setDate(const void *date)
{
    return EV_set_date((ST_DATE *)date);
}





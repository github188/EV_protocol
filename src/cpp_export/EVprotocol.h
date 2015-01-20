
#ifndef _EVprotocol_H_
#define _EVprotocol_H_

#ifdef __cplusplus
extern "C" {
#endif



#define EV_EXPORT __declspec(dllexport)
#ifdef EV_WIN32
#define EV_API  __stdcall //注意在win32平台下统一采用stdcall标准调用方式
#else
#define EV_API
#endif


/*********************************************************************************************************
** Function name	:		EV_CALLBACK_HANDLE
** Descriptions		:		回调函数入口定义
** input parameters	:       type 回调回传的类型, ptr 回调回传的结构体参数
** output parameters:		无
** Returned value	:		无
*********************************************************************************************************/
typedef void (EV_API *EV_CALLBACK_HANDLE)(int type,const void *ptr);








/*********************************************************************************************************
** Function name	:		EV_vmcStart
** Descriptions		:		启动VMC服务程序
** input parameters	:       portName 串口号 例如"COM1", callBack 回调函数指针
** output parameters:		无
** Returned value	:		1成功  -1失败(串口打开失败)  0创建线程失败
*********************************************************************************************************/
EV_EXPORT int   EV_API  EV_vmcStart(char *portName,EV_CALLBACK_HANDLE callBack);




EV_EXPORT void  EV_API  EV_vmcStop();
EV_EXPORT int   EV_API  EV_trade(int cabinet,int column,int type,long cost);
EV_EXPORT int   EV_API  EV_payout(long value);
EV_EXPORT int   EV_API  EV_getStatus();
EV_EXPORT long  EV_API  EV_getRemainAmount();
EV_EXPORT int   EV_API  EV_bentoRegister(char *portName);
EV_EXPORT int   EV_API  EV_bentoOpen(int cabinet, int box);
EV_EXPORT int   EV_API  EV_bentoRelease();
EV_EXPORT int   EV_API  EV_bentoLight(int cabinet, int flag);
EV_EXPORT int   EV_API  EV_bentoCheck(int cabinet,char *msg);
EV_EXPORT int   EV_API  EV_cashControl(int flag);
EV_EXPORT int   EV_API  EV_cabinetControl(int cabinet, int dev, int flag);
EV_EXPORT int   EV_API  EV_setDate(const void *date);



#ifdef __cplusplus
}
#endif
#endif

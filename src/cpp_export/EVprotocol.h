
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

typedef void (*EV_CALLBACK_HANDLE)(int type,void *ptr);



typedef struct _ev_date_{
    int year;
    int moth;
    int day;
    int hour;
    int min;
    int sec;
    int week;
}EV_DATE;





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
EV_EXPORT int   EV_API  EV_setDate(const EV_DATE *date);



#ifdef __cplusplus
}
#endif
#endif

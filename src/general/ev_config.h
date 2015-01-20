#ifndef _EV_CONFIG_H_
#define _EV_CONFIG_H_



typedef unsigned char       quint8;
typedef unsigned short      quint16;
typedef unsigned int        quint32;
typedef unsigned long       quint64;

typedef  char       qint8;
typedef  short      qint16;
typedef  int        qint32;
typedef  long       qint64;


//适用于安卓的特定定位输出
#if EV_ANDROID
#include<android/log.h>
#define EV_LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "EV_thread", __VA_ARGS__))
#define EV_LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))
#define EV_LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "EV_thread", __VA_ARGS__))
#define EV_LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "EV_thread", __VA_ARGS__))
//7级日志输出  数字越小级别越高
#define EV_LOGI1(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))
#define EV_LOGI2(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))
#define EV_LOGI3(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))
#define EV_LOGI4(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))
#define EV_LOGI5(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))
#define EV_LOGI6(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))
#define EV_LOGI7(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))
#define EV_LOGCOM(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))

#define EV_LOGTASK(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))
#define EV_LOGFLOW(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))


#else
#define EV_LOGD(...)    printf(__VA_ARGS__)
#define EV_LOGI(...)    printf(__VA_ARGS__)
#define EV_LOGW(...)    printf(__VA_ARGS__)
#define EV_LOGE(...)    printf(__VA_ARGS__)
#define EV_LOGI1(...)   printf(__VA_ARGS__)
#define EV_LOGI2(...)   printf(__VA_ARGS__)
#define EV_LOGI3(...)   printf(__VA_ARGS__)
#define EV_LOGI4(...)   printf(__VA_ARGS__)
#define EV_LOGI5(...)   printf(__VA_ARGS__)
#define EV_LOGI6(...)   printf(__VA_ARGS__)
#define EV_LOGI7(...)   printf(__VA_ARGS__)
#define EV_LOGCOM(...)  do{}while(0)//屏蔽串口十六进制打印
#define EV_LOGTASK(...) printf(__VA_ARGS__)
#define EV_LOGFLOW(...) printf(__VA_ARGS__)

#endif



/*********************************************************************************************************
**定义通用宏函数
*********************************************************************************************************/

#define HUINT16(v)   	(((v) >> 8) & 0xFF)
#define LUINT16(v)   	((v) & 0xFF)
#define INTEG16(h,l)  	(((quint32)h << 8) | l)
#define H0UINT32(n)				(quint8)(((n) >> 24) & 0xff) //32位数值第一个高8位
#define H1UINT32(n)				(quint8)(((n) >> 16) & 0xff) //32位数值第二个高8位
#define L0UINT32(n)				(quint8)(((n) >> 8 ) & 0xff) //32位数值第一个低8位
#define L1UINT32(n)				(quint8)(((n) >> 0 ) & 0xff)         //32位数值第二个低8位
#define INTEG32(h0,l0,h1,l1)	(quint32)(((quint32)(h0) << 24)| \
                                               ((quint32)(l0) << 16)|  \
                                               ((quint32)(h1) << 8 )|((l1) & 0xff))
							    						    
									



quint16 EV_crcCheck(quint8 *msg,quint8 len);

#endif

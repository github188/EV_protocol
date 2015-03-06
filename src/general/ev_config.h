#ifndef _EV_CONFIG_H_
#define _EV_CONFIG_H_
#include "EVprotocol.h"
#include "LOGC.h"


//适用于安卓的特定定位输出
#if 1
#if (defined EV_ANDROID)
#include<android/log.h>
#define EV_LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "EV_thread", __VA_ARGS__))
#define EV_LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))
#define EV_LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "EV_thread", __VA_ARGS__))
#define EV_LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "EV_thread", __VA_ARGS__))
#define EV_LOGCOM(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))
#define EV_LOGTASK(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))
#define EV_LOGFLOW(...) ((void)__android_log_print(ANDROID_LOG_INFO, "EV_thread", __VA_ARGS__))

#elif (defined EV_WIN32)
#define EV_LOGD(...)    (DebugLog( __FILE__ , __LINE__ , __VA_ARGS__ ))
#define EV_LOGI(...)    (InfoLog( __FILE__ , __LINE__ , __VA_ARGS__ ))
#define EV_LOGW(...)    (WarnLog( __FILE__ , __LINE__ , __VA_ARGS__ ))
#define EV_LOGE(...)    (ErrorLog( __FILE__ , __LINE__ , __VA_ARGS__ ))
#define EV_LOGCOM(...)  (InfoLog( __FILE__ , __LINE__ , __VA_ARGS__ ))
#define EV_LOGTASK(...) (InfoLog( __FILE__ , __LINE__ , __VA_ARGS__ ))
#define EV_LOGFLOW(...) (InfoLog( __FILE__ , __LINE__ , __VA_ARGS__ ))

#else
#define EV_LOGD(...)    do{}while(0)
#define EV_LOGI(...)    do{}while(0)
#define EV_LOGW(...)    do{}while(0)
#define EV_LOGE(...)    do{}while(0)
#define EV_LOGCOM(...)  do{}while(0)
#define EV_LOGTASK(...) do{}while(0)
#define EV_LOGFLOW(...) do{}while(0)

#endif
#endif


/*********************************************************************************************************
**定义通用宏函数
*********************************************************************************************************/
#define HUINT16(v)   	(((v) >> 8) & 0xFF)
#define LUINT16(v)   	((v) & 0xFF)
#define INTEG16(h,l)  	(((uint32)h << 8) | l)
#define H0UINT32(n)				(uint8)(((n) >> 24) & 0xff) //32位数值第一个高8位
#define H1UINT32(n)				(uint8)(((n) >> 16) & 0xff) //32位数值第二个高8位
#define L0UINT32(n)				(uint8)(((n) >> 8 ) & 0xff) //32位数值第一个低8位
#define L1UINT32(n)				(uint8)(((n) >> 0 ) & 0xff)         //32位数值第二个低8位
#define INTEG32(h0,l0,h1,l1)	(uint32)(((uint32)(h0) << 24)| \
                                               ((uint32)(l0) << 16)|  \
                                               ((uint32)(h1) << 8 )|((l1) & 0xff))
							    						    
									



uint16 EV_crcCheck(uint8 *msg,uint8 len);
int EV_createLog();
#endif

#ifndef _EV_TIMER_H_
#define _EV_TIMER_H_

#include <pthread.h>
#include "ev_config.h"


typedef void (*EV_timerISR)(void);

typedef struct _st_timer_{
    quint8 start;
    int id; //定时器ID
    EV_timerISR isr;//定时器服务函数
    volatile quint32 tick;
}ST_TIMER;

typedef struct _TIMER_NODE_{
    ST_TIMER *timer;
    struct TIMER_NODE *next;
}TIMER_NODE;



int EV_timer_register(ST_TIMER *timer);
void EV_timer_release(const ST_TIMER *timer);
quint8 EV_timer_start(ST_TIMER *timer,quint32 sec);
void EV_timer_stop(ST_TIMER *timer);

void EV_msleep(unsigned long msec);//毫秒睡眠
#endif

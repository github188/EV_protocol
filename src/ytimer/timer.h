#ifndef _EV_TIMER_H_
#define _EV_TIMER_H_

#include <pthread.h>
#include "ev_config.h"




typedef void (*EV_timerISR)(void);

typedef struct st_timer_{
    uint8 start;
    int id; //定时器ID
    EV_timerISR isr;//定时器服务函数
    volatile uint32 tick;
    volatile uint32 varTick;
}ST_TIMER;


struct ST_TIMER_NODE{
    ST_TIMER *timer;
    struct ST_TIMER_NODE *next;
};

typedef struct ST_TIMER_NODE TIMER_NODE;



int EV_timer_register(EV_timerISR timer_isr);
void EV_timer_release(int timerId);
uint8 EV_timer_start(int timerId,uint32 sec);
void EV_timer_stop(int timerId);
uint8 EV_timer_isTimeout(int timerId);
void EV_msleep(unsigned long msec);//毫秒睡眠
#endif

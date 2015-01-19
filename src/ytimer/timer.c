#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/time.h> //定时器
#include <signal.h>
#endif



static pthread_t timer_pid = 0;
static char timerStopped = 0;

static TIMER_NODE timer_node;

void *EV_timer_run(void *ptr)
{
    TIMER_NODE *q,*p;
    ST_TIMER *timer;
    if(ptr == NULL)
    {
        pthread_exit(0);
        return NULL;
    }
    while(timerStopped == 0)
    {
        EV_msleep(100);
        p = (TIMER_NODE *)ptr;
        while(p->next != NULL)
        {
            q = p->next;
            p = q;
            timer = q->timer;
            if(timer == NULL) continue;
            if(timer->tick) timer->tick--;
            else
            {
                if(timer->isr && timer->start == 1)
                {
                    timer->start = 0;
                    timer->isr();
                }
            }
        }

    }
     printf("pthread_exit\n");
    pthread_exit(0);
    return NULL;
}



//注册定时器100ms的定时器  成功返回定时器ID号  失败返回 -1
int EV_timer_register(ST_TIMER *timer)
{
    int id;
    TIMER_NODE *p,*q,*t;
    if(timer == NULL) return -1;
    //定时器精度为100毫秒级
    if(timer_pid == 0)//还没开启线程 线开启定时器线程
    {
        timer_node.next = NULL;//首节点
        timer_node.timer = NULL;
        timerStopped = 0;
        id = pthread_create(&timer_pid,NULL,EV_timer_run,(void *)&timer_node);
        if(id != 0)
        {
            printf("thread create failed..\n");
            return -1;
        }

    }

    timer->start = 0;
    timer->tick = 0;
    printf("register....\n");
    //创建一个定时器结构节点
    t = (TIMER_NODE *)malloc(sizeof(TIMER_NODE));
    if(t == NULL)
        return -1;
    t->next = NULL;
    t->timer = timer;
    //插入链表
    p = &timer_node;
    while(p->next != NULL)
    {
        q = p;
        p = q->next;
        printf("ex_timer:%d\n",p->timer->id);
    }
    p->next = t;
    printf("register OK\n");
    return timer->id;
}

//彻底关掉定时器
void EV_timer_release(const ST_TIMER *timer)
{
    TIMER_NODE *p = &timer_node,*q,*s;
    if(timer == NULL) return;
    while(p->next != NULL)
    {
        s = p->next;
        if(s->timer == timer)//找到了定时器
        {
            printf("free timer:%d\n",s->timer->id);
            p->next = s->next;//下下一个定时器
            free(s);//释放内存
            break;
        }
        else
        {
            q = p;
            p = q->next;
        }

    }
    p = &timer_node;
    while(p->next != NULL)
    {
        q = p->next;
        p = q;
        printf("re_timer:%d\n",q->timer->id);
    }
    if(timer_node.next == NULL)//没有定时器了 则杀掉线程
    {
        timerStopped = 1;
        timer_pid = 0;
        printf("all timer killed\n");
    }

}


void EV_timer_stop(ST_TIMER *timer)
{
    if(timer == NULL) return;
    timer->start = 0;


}


quint8 EV_timer_start(ST_TIMER *timer,quint32 sec)
{
    if(timer == NULL) return 0;
 //   pthread_mutex_lock(&timer->mutex);
    timer->tick = (sec * 10);
    timer->start = 1;
  //  pthread_mutex_unlock(&timer->mutex);

    return 1;
}



void EV_msleep(unsigned long msec)//毫秒睡眠
{
#ifdef _WIN32
    Sleep(msec);
#else
    usleep(msec * 1000);
#endif
}




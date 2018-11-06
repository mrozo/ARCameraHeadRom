
/*
 * TimerEventDispatcher.h
 *
 * Created: 2014-11-15 20:46:31
 *  Author: m
 */ 

#ifndef _TIMER_EVENT_DISPATCHER_H_
#define _TIMER_EVENT_DISPATCHER_H_

#include "TimerDriver.h"

#ifndef TIMER_EVENTS_LIST_LENGTH
#define TIMER_EVENTS_LIST_LENGTH 8
#endif

#ifndef TIMER_TIMEOUT
#define TIMER_TIMEOUT MICROSECONDS_PERIOD(500)
#endif

class TimerEventDispatcher{
  public:
  
    TimerEventDispatcher();
    
    TIMER_EVENT *CreateEvent(
        TimerEventType type,
        NANOSECONDS time,
        TIMER_CALLBACK callback,
        void *context
      );
    void DestroyEvent(TIMER_EVENT *e);
    void SysTimerOverflow();
    void CheckEvents();
  private:
    TimerDriver _driver;
    UINT _eventListLength = TIMER_EVENTS_LIST_LENGTH;
    NANOSECONDS _sysTimerTimeout = TIMER_TIMEOUT;
    TIMER_EVENT _events[TIMER_EVENTS_LIST_LENGTH];
    TIMER_EVENT* _GetFreeEventSlot();
  
};

void InitTimerEventDispatcher();


#endif
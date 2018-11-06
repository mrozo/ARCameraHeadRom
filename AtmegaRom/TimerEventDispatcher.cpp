/*
 * TimerEventDispatcher.c
 *
 * Created: 2014-11-15 18:57:20
 *  Author: m
 */

#include "Standarts.h"
#include "TimerDriver.h"
#include "TimerEventDispatcher.h"
#include "GlobalServicesTable.h"
#include <string.h>

extern GLOBAL_SERVICES_TABLE gST;

TimerEventDispatcher::TimerEventDispatcher(){
  memset(_events,0,sizeof(_events));
  _driver = TimerDriver();
  _driver.SetClockSource(Internal);
  _driver.SetOverflowInterval(_sysTimerTimeout);
  _driver.Start();
}


TIMER_EVENT* TimerEventDispatcher::_GetFreeEventSlot()
{
  for(UINT i=0; i<_eventListLength; i++){
    if( Closed == _events[i].type )
      return &_events[i];
  }
  return NULL;
}

TIMER_EVENT *TimerEventDispatcher::CreateEvent(
  TimerEventType type,
  NANOSECONDS time,
  TIMER_CALLBACK callback,
  void *context
){
  TIMER_EVENT *e = _GetFreeEventSlot();
  
  if(e == NULL)
    return NULL;
  
  e->time = time;
  e->eplased = time;
  e->Callback = callback;
  e->context = context;
  e->type = type;
  return e;
}

void TimerEventDispatcher::DestroyEvent(TIMER_EVENT *e){
  e->type = Closed;
}


void TimerEventDispatcher::SysTimerOverflow()
{
  TIMER_EVENT *e;
  for(UINT i=0; i < _eventListLength ;i++ ){
    e = &_events[i];
    if(Closed != e->type && e->eplased>=_sysTimerTimeout){
      e->eplased -= _sysTimerTimeout;
      if(e->type == Interval && e->eplased<_sysTimerTimeout){
        e->eplased = e->time;
        e->Callback(e,e->context);
      }         
    }
  }
}

void TimerEventDispatcher::CheckEvents()
{
  TIMER_EVENT *e;PORTB ^=0xC0;
  for( UINT i=0; i < _eventListLength ;i++ ){
    e = &_events[i];
    if(e->type==ExternalInteval && e->eplased < _sysTimerTimeout){
      e->eplased = e->time;
      e->Callback(e,e->context);
        
    }  
  }    
}

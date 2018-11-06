/*
 * GlobalSystemTable.h
 *
 * Created: 2014-11-16 14:40:51
 *  Author: m
 */ 
#ifndef _GLOBAL_SYSTEM_TABLE_H_
#define _GLOBAL_SYSTEM_TABLE_H_

#include "Standarts.h"
#include "SerialCommunication.h"
#include "TimerEventDispatcher.h"

typedef struct _GLOBAL_SERVICES_TABLE{
  SerialCommunication *serial;
  TimerEventDispatcher *timer;
} GLOBAL_SERVICES_TABLE;

Status InitGlobalServicesTable();

#endif

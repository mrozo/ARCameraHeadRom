/*
 * DebugLib.cpp
 *
 * Created: 2014-11-15 22:48:32
 *  Author: m
 */ 

#include "DebugLib.h"

volatile bool VolatileTrue = true;

void DeadLoop(){
  while(VolatileTrue){}
}
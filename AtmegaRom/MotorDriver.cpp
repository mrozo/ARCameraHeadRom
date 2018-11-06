/* 
* MotorDriver.cpp
*
* Created: 2014-11-26 18:25:21
* Author: m
*/

#define ABS(x) (((x)<0)?(-(x)):((x)))

#include "MotorDriver.h"
#include "DebugLib.h"

void DispatchMotorDriverEvent(TIMER_EVENT *e, void *c){
  ((MotorDriver*) c)->TimerEventHandler();
}

// default constructor
MotorDriver::MotorDriver()
{
} //MotorDriver

// default destructor
MotorDriver::~MotorDriver()
{
} //~MotorDriver


void MotorDriver::SetMotorStates(MOTOR_STATES *states)
{
	_states = states;
}

void MotorDriver::SetGearRatio(UINT ratio)
{
	_ratio = ratio;
}

void MotorDriver::SetStepsPerRevolution(uint16_t steps)
{
	_stepsPerRevolution = steps;
}

void MotorDriver::Start()
{
	_intervalEvent = gST.timer->CreateEvent(
                                  ExternalInteval,
                                  _timeout,
                                  DispatchMotorDriverEvent,
                                  (void*)this
                              );
}

void MotorDriver::TimerEventHandler()
{
  if(ABS(_position - _desiredPosition)<=_treshold){
    HitBreak();     
  } else if(_desiredPosition<_position) {
    StepLeft();
  } else {
    StepRight();
  } 
}

void MotorDriver::SetTreshold(ANGLE a)
{
	_treshold = a;
}

void MotorDriver::HitBreak()
{
	//_states->StateUpdateMethod(_states->BreakState);
}

void MotorDriver::StepRight()
{
  
  if( _position > _rightAngleLimit){
    HitBreak();
    return;
  }

_currentStateNumber++;
if(_currentStateNumber>=_states->StepsNumber)
  _currentStateNumber=0;

_states->StateUpdateMethod(_states->Steps[_currentStateNumber]);
_position ++;

}

void MotorDriver::StepLeft()
{
  if( _position < _leftAngleLimit){
    HitBreak();
    return;
  }

  if(_currentStateNumber==0)
    _currentStateNumber=_states->StepsNumber-1;
  else
    _currentStateNumber--;
  
  _states->StateUpdateMethod(_states->Steps[_currentStateNumber]);
  _position--;
    
}

void MotorDriver::SetLimitAngles(ANGLE left, ANGLE right)
{
	_leftAngleLimit = left;
  _rightAngleLimit = right;
}

void MotorDriver::SetAngle(ANGLE a)
{
	_desiredPosition = a;
}

ANGLE MotorDriver::GetAngle()
{
	return _position;
}

void MotorDriver::SetZero()
{
  _desiredPosition=0;
	_position = 0;
}

void MotorDriver::SetTimeot(NANOSECONDS timeout)
{
	_timeout = timeout;
}

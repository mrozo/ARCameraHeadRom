/* 
* MotorDriver.h
*
* Created: 2014-11-26 18:25:21
* Author: m
*/


#ifndef __MOTORDRIVER_H__
#define __MOTORDRIVER_H__

#include "Standarts.h"
#include "GlobalServicesTable.h"

extern GLOBAL_SERVICES_TABLE gST;

typedef void (*MOTOR_STATE_UPDATE_METHOD)(UINT state);

typedef struct _MOTOR_STATES{
  MOTOR_STATE                BreakState;
  MOTOR_STATE                Loose;
  MOTOR_STATE                Steps[32];
  UINT                       StepsNumber;
  MOTOR_STATE_UPDATE_METHOD  StateUpdateMethod;
} MOTOR_STATES;

class MotorDriver
{
//variables
public:
protected:
private:
  volatile int32_t _position          = 0;
  volatile int32_t _desiredPosition   = 0;
  volatile int32_t _zeropPosition     = 0;
  volatile UINT _stepsPerRevolution   = 1;
  volatile UINT _ratio                = 1;
  volatile int32_t _treshold            = 2;
  NANOSECONDS _timeout = MICROSECONDS_PERIOD(1500);
  TIMER_EVENT *_intervalEvent= NULL;
  MOTOR_STATES *_states;
  volatile int _currentStateNumber = 0;
  volatile int32_t _leftAngleLimit;
  volatile int32_t _rightAngleLimit;
//functions
public:
	MotorDriver();
	~MotorDriver();
  
  void SetAngle(ANGLE a);
  ANGLE GetAngle();
  
  void SetMotorStates( MOTOR_STATES *States);
  void SetGearRatio(UINT ratio);
  void SetStepsPerRevolution(uint16_t steps);
  void SetLimitAngles(ANGLE left, ANGLE right);
  void SetTimeot(NANOSECONDS timeout);

  void TimerEventHandler();
  void Start();
  void SetTreshold(ANGLE a);
  void HitBreak();
  void StepLeft();
  void StepRight();
  void SetZero();
  MotorDriver( const MotorDriver &c );
  MotorDriver& operator=( const MotorDriver &c );

protected:
private:


}; //MotorDriver

#endif //__MOTORDRIVER_H__

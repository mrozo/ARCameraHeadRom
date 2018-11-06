/*
 * Standarts.h
 *
 * Created: 2014-11-01 21:30:49
 *  Author: m
 */ 

#ifndef _STANDARTS_H_
#define _STANDARTS_H_

#include <avr/io.h>
#include <avr/interrupt.h>

typedef uint8_t MOTOR_STATE;
typedef uint8_t *PORT;
typedef int32_t ANGLE;
typedef uint8_t BYTE;
typedef uint8_t UINT;
typedef uint32_t NANOSECONDS;


#define CP_BITS(source,destination,mask) ((destination) & (~(mask))) | ((source)&(mask));
#define SET(byte,mask) (byte)|(mask)
#define CLR(byte,mask) (byte)&(~(mask))
#define TOGGLE(byte,mask) (byte)^(mask)
#define NUMBER2MASK(number) (0x1 << (number))
#define BIT_VAL(byte,bitNumber) (((1<<(bitNumber))&(byte))!=0)
#define MICROSECONDS_PERIOD(number) (NANOSECONDS)1000 * (NANOSECONDS)number
#define MILISECONDS_PERIOD(number) (NANOSECONDS)1000000 * (NANOSECONDS)number
#define SECONDS_PERIOD(number) (NANOSECONDS)1000000000 * (NANOSECONDS)number

#define NULL 0x00

enum ClockSource{
  Internal,
  External
};

enum PrescalerValues{
  NoPrescaling = 1,
  TwoToThePowerOfThree = 2,
  TwoToThePowerOfSix = 3,
  TwoToThePowerOfEight = 4,
  TwoToThePowerOfTen = 5
};

enum EdgeSelect{
  Falling = 0,
  Rising = 1
};

enum DataSize{
  FiveBits,
  SixBits,
  SevenBits,
  EightBits,
  NineBits
};

enum SerialMode{
  Async,
  Sync,
  SpiMaster
};
  
enum Parity{
  Disabled,
  Even,
  Odd
};
  
enum StopBits{
  One,
  Two
};

enum Status{
  Success,
  Fail,
  OutOfResources,
  UnknownError,
  InvalidParameter
};


enum TimerEventType{
  Closed=0,
  Interval,
  Timeout,
  ExternalInteval
};

enum ConnectionStatus{
  Stopped=1,
  WaitingForHandshake=2,
  Handshaking=3,
  Active=4,
  Broken=5,
  WaitingForKeepAliveResponse=6,
  WaitingForKeepAliveByte=7
  };

typedef struct _TIMER_EVENT TIMER_EVENT;

typedef void (*TIMER_CALLBACK)(TIMER_EVENT*,void*);

typedef struct _TIMER_EVENT{
  TimerEventType type;
  NANOSECONDS time;
  NANOSECONDS eplased;
  TIMER_CALLBACK Callback;
  void *context;
}TIMER_EVENT;

#endif

/*
 * AtmegaRom.cpp
 *
 * Created: 2014-11-04 22:23:30
 *  Author: m
 */ 

#include "Standarts.h"
#include "DebugLib.h"
#include "GlobalServicesTable.h"
#include "TimerEventDispatcher.h"
#include "SerialCommunication.h"
#include "MotorDriver.h"
#include <avr\delay.h>
extern GLOBAL_SERVICES_TABLE gST;


#define HORIZONTAL_MOTOR_STEPS_PER_ROTATION (ANGLE)(2*200*14.4)
#define VERTICAL_MOTOR_STEPS_PER_ROTATION (ANGLE)(2*200*7.2)
#define HORIZONTAL_MOTOR_ANGLE_PARAMETER (ANGLE)(0xB60B6)
#define VERTICAL_MOTOR_ANGLE_PARAMETER (ANGLE)(0x16C16C)

void HorizontalMotorUpdate(UINT state){ PORTB = (PINB&0xC0)|(state&0x3f);}
void VerticalMotorUpdate(UINT state){ PORTC = (state&0x3f)|(PINC&0xC0);}
MOTOR_STATES verticalMotorStates = {
  0x03,
  0x00,
  //{0x01<<2,0x08<<2, 0x02<<2, 0x10<<2},
  {0x04,0x14,0x10,0x18, 0x08,0x28,0x20,0x24 },
  8,
  VerticalMotorUpdate,
};

MOTOR_STATES horizontalMotorStates = {
  0x03,
  0x00,
  //{0x01<<2,0x08<<2, 0x02<<2, 0x10<<2},
  {0x01, 0x05,0x04,0x06,0x02,0x0A,0x08,0x09 },
  8,
  HorizontalMotorUpdate
};

MotorDriver verticalMotor = MotorDriver();
MotorDriver horizontalMotor= MotorDriver();

void WaitForTHeCommunication()
{
	while( gST.serial->GetState() != Active ){}
}

void SetUpVerticalMotor()
{
  verticalMotor.SetMotorStates(&verticalMotorStates);
  verticalMotor.SetGearRatio(1);
  verticalMotor.SetStepsPerRevolution(400);
  verticalMotor.SetTreshold(0);
  verticalMotor.SetLimitAngles(
    -VERTICAL_MOTOR_STEPS_PER_ROTATION/4,
    VERTICAL_MOTOR_STEPS_PER_ROTATION/4
  );
  verticalMotor.SetTimeot(MICROSECONDS_PERIOD(1000));
  verticalMotor.Start();
}

void SetUpHorizontalMotor()
{
	horizontalMotor.SetMotorStates(&horizontalMotorStates);
  horizontalMotor.SetGearRatio(1);
  horizontalMotor.SetStepsPerRevolution(200);
  horizontalMotor.SetTreshold(0);
  horizontalMotor.SetLimitAngles(
    -HORIZONTAL_MOTOR_STEPS_PER_ROTATION/2,
    HORIZONTAL_MOTOR_STEPS_PER_ROTATION/2
  );
  horizontalMotor.SetTimeot(MICROSECONDS_PERIOD(1000));
  horizontalMotor.Start();
}

typedef struct{
  uint32_t h;
  uint32_t v;
} ANGLES;

union PACKET{
  BYTE raw[10];
  ANGLES angles;
};

PACKET p;

void SendHeadStatus(){
  p.angles.h = (uint32_t) (
        (horizontalMotor.GetAngle() + HORIZONTAL_MOTOR_STEPS_PER_ROTATION/2 )
        * (ANGLE)(0xFFFFFFFF/HORIZONTAL_MOTOR_STEPS_PER_ROTATION)
      );
 
 
  p.angles.v = (uint32_t) ((
        (verticalMotor.GetAngle()   + VERTICAL_MOTOR_STEPS_PER_ROTATION/2)
        * (ANGLE)(0xFFFFFFFF/VERTICAL_MOTOR_STEPS_PER_ROTATION)
      ));
  gST.serial->SendPacket((p.raw));
}

void PacketReceived(BYTE packet[10]){
  PACKET *p = (PACKET*)((void*)packet);
  horizontalMotor.SetAngle(
    (p->angles.h / HORIZONTAL_MOTOR_ANGLE_PARAMETER  ) - HORIZONTAL_MOTOR_STEPS_PER_ROTATION/2 );
  verticalMotor.SetAngle(
    (p->angles.v / VERTICAL_MOTOR_ANGLE_PARAMETER ) - VERTICAL_MOTOR_STEPS_PER_ROTATION/2) ;
}

void SetUpMotors()
{
	SetUpVerticalMotor();
  SetUpHorizontalMotor();
}

void SetupPorts()
{
	DDRB = 0xFF;
  DDRD = 0x00;
  DDRC = 0xFF;
  PORTC = 0x00;
  PORTD = 0xFF;
  PORTB= 0x00;
}


void TheLoop()
{  
  gST.timer->CheckEvents();
  
  if(BIT_VAL(UCSR0A,RXC0)){
    gST.serial->ReceiveData();
  }    
  
  if((PIND&0xFC)!=0xFC){
    if((~PIND)&(1<<7)){
      horizontalMotor.SetZero();
      verticalMotor.SetZero();
    }
    if((~PIND)&(1<<6)){
      verticalMotor.SetAngle( verticalMotor.GetAngle() + 64 );
    }	
    if((~PIND)&(1<<5)){
      horizontalMotor.SetAngle(0);
      verticalMotor.SetAngle(0);
    }
    if((~PIND)&(1<<4))
      horizontalMotor.SetAngle( horizontalMotor.GetAngle() - 64 );
	
    if((~PIND)&(1<<3)){
      verticalMotor.SetAngle( verticalMotor.GetAngle() - 64 );
    }	
    if((~PIND)&(1<<2))
      horizontalMotor.SetAngle( horizontalMotor.GetAngle() + 64 );
  }  

}

int main(void)
{
  SetupPorts();
  InitGlobalServicesTable();
  SetUpMotors();
  while(true){
    //inicjalizacja protoko³u
    gST.serial->Start();
    //przekazanie uchwytu do funkcji generuj¹cej pakiety do nadania
    gST.serial->TxBufferSentEventHandler = SendHeadStatus; 
    //wymuszenie generacjii pierwszego pakietu
    SendHeadStatus();
    //g³ówna pêtla programu
    while(gST.serial->GetState() != Broken){
      TheLoop();
    }
    //zerowanie parametrów uk³adu
    horizontalMotor.SetAngle(0);
    verticalMotor.SetAngle(0);
    _delay_ms(3000);
  }
}

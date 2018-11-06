/*
 * SerialCommunication.cpp
 *
 * Created: 2014-11-15 20:52:03
 *  Author: m
 */ 

#include "SerialCommunication.h"
#include "GlobalServicesTable.h"
#include "DebugLib.h"

#define IS_RX_DATA_AVAIABLE() (BIT_VAL(UCSR0A,RXC0))
#define IS_TX_BUFFER_READY() (BIT_VAL(UCSR0A,TXC0))
extern GLOBAL_SERVICES_TABLE gST;
extern BYTE UCSR0B_copy;
extern void PacketReceived(BYTE packet[10]);

 SerialCommunication::SerialCommunication()
{
  _state = Stopped;
}

ConnectionStatus SerialCommunication::GetState()
{
  return _state;
}

void SerialCommunication::_HandshakeState()
{
  if(_handshakeStep<4){
    if (IS_RX_DATA_AVAIABLE()){
        if(UDR0 == PONG_SIGNAL)
          _handshakeStep++;
        else
          _state = WaitingForHandshake;      
      } else
        _state = WaitingForHandshake;     
    } else if(_handshakeStep<9){
      if( IS_RX_DATA_AVAIABLE() )
        _state = WaitingForHandshake;
      else
         _handshakeStep++;
    } else if(_handshakeStep<14){
      if (IS_RX_DATA_AVAIABLE()){
        UDR0 = (UDR0+1);
        _handshakeStep++;
      } else
        _state = WaitingForHandshake;
    } else {    
      _ticsFromLastReceivedByte=1;
      _ticsFromLastTransmittedByte=1;
      _state = Active;
    }
}

void SerialCommunication::_SetupSerial()
{
  _serial.SetDataSize(EightBits);
  _serial.SetBaudRate(9600);
  _serial.SetMode(Async);
  _serial.SetParity(Disabled);
  _serial.SetStopBits(One);
  _serial.EnableCommunication();
  _serial.SendStatusRegisters();
}


void SerialCommunication::_WaitingForHandshakeState()
{
  BYTE input;
  if( IS_RX_DATA_AVAIABLE() ){
    if( UDR0 == PONG_SIGNAL ){
      _handshakeStep = 0;
      _state = Handshaking;
    } else
      UDR0 = PING_SIGNAL;
  } else
    UDR0 = PING_SIGNAL;
}

void SerialCommunication::ManageCommunication(TIMER_EVENT *e, void *c)
{
  switch(_state){
    case WaitingForHandshake:           _WaitingForHandshakeState();  break;
    case Handshaking:                   _HandshakeState();            break;
    case Broken:                        _BrokenState();               break;
    
    case WaitingForKeepAliveResponse :
    case WaitingForKeepAliveByte     :
    case Active:                        
      _ActiveState();
      break;
    default: return;
  }
}


void SerialCommunication::ReceiveData(){
  BYTE input;
  static bool lock = false;
  if( lock || !IS_RX_DATA_AVAIABLE() )
    return;
  else
    lock=true;

  _ticsFromLastReceivedByte=0;
  
  if(_state == WaitingForKeepAliveByte)
    CheckForKeepAliveByte();
  else if(_state == WaitingForKeepAliveResponse)
    CheckForKeepAliveResponse();
  else if (_state == Active) {
    _ReceiveByte();
    _SendByte();
  }

  lock = false;
}  

Status SerialCommunication::Start()
{
  _SetupSerial();
  _txDataAvaliable = false;
  _txBufferPosition = 0;
  _txBuffer = 0;
  _state = WaitingForHandshake;
  return Success;
}

void SerialCommunication::_ActiveState()
{
  _CheckRxLineState();
  if(_state==Active)
    _CheckTxLineState();

}

void SerialCommunication::_BrokenState()
{
	if(_autoRestore)
    RestoreConnection();
}

void SerialCommunication::SendPacket(BYTE *packet)
{
  if(_txDataAvaliable)
    return;
    
	_txBuffer = packet;
  _txBufferPosition=0;
  _txDataAvaliable = true;
}

void SerialCommunication::RestoreConnection()
{
  _ClearData();
  _state = WaitingForHandshake;
}

void SerialCommunication::_ClearData()
{
  _txDataAvaliable = false;
  _txBufferPosition = 0;
  _txBuffer = NULL;
  _rxBufferPosition = 0;
  _handshakeStep = 0;
}

void SerialCommunication::CheckForKeepAliveResponse()
{
  if(UDR0 == KEEP_ALIVE_BYTE)
    _state = Active;
  else
    _state = Broken;
}

void SerialCommunication::CheckForKeepAliveByte()
{
  if(UDR0 == KEEP_ALIVE_BYTE){
    _ticsFromLastTransmittedByte=0;
    UDR0 = KEEP_ALIVE_BYTE;
    _state = Active;
    } else
    _state = Broken;
}

void SerialCommunication::_ReceiveByte()
{
	_rxBuffer[_rxBufferPosition] = UDR0;
    _rxBufferPosition++;
    _ticsFromLastReceivedByte = 0;
    if(_rxBufferPosition==_bytesPerPacket){
      _rxBufferPosition = 0;
      PacketReceived(_rxBuffer);
    }
}

void SerialCommunication::_SendByte()
{
	if(_txDataAvaliable){
      UDR0 = _txBuffer[_txBufferPosition];
      _txBufferPosition++;
      _ticsFromLastTransmittedByte = 0;
      if(_txBufferPosition==_bytesPerPacket){
        _txDataAvaliable = false;
        if(TxBufferSentEventHandler!=NULL)
        TxBufferSentEventHandler();
      }
    }
}

void SerialCommunication::_CheckRxLineState()
{
	_ticsFromLastReceivedByte++;
  if(_ticsFromLastReceivedByte==5)
    _state = WaitingForKeepAliveByte;
  else if(_ticsFromLastReceivedByte==10){
    _state = Broken;
  }
}

void SerialCommunication::_CheckTxLineState()
{
	_ticsFromLastTransmittedByte++;
  if(_ticsFromLastTransmittedByte==4){
    UDR0 = KEEP_ALIVE_BYTE;
    _state = WaitingForKeepAliveResponse;
  } else if (_ticsFromLastTransmittedByte==10){
    _state = Broken;
  }
}

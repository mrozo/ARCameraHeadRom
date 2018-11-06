
/*
 * SerialCommunication.h
 *
 * Created: 2014-11-15 20:52:23
 *  Author: m
 */ 

#ifndef _SERIAL_COMMUNICATION_H_
#define _SERIAL_COMMUNICATION_H_

#include "Standarts.h"
#include "SerialDriver.h"
#include "DebugLib.h"

#define PING_SIGNAL 0x11
#define PONG_SIGNAL 0x11
#define KEEP_ALIVE_BYTE 0x55

typedef void (*TX_BUFFER_SENT)() ;
class SerialCommunication{
  public:
  TX_BUFFER_SENT TxBufferSentEventHandler = NULL;
    SerialCommunication();
    Status Start();
    ConnectionStatus GetState();
    //void TransmitNextByte();
    void ManageCommunication(TIMER_EVENT *e, void *c);
    void ReceiveData();
    void _SendByte();
    void _ReceiveByte();
    void CheckForKeepAliveByte();
    void CheckForKeepAliveResponse();
    void SendPacket(BYTE *packet);
    void RestoreConnection();
  private:
    volatile ConnectionStatus _state = Stopped;
    volatile UINT _handshakeStep = 0;
    volatile UINT _ticsFromLastReceivedByte = 0;
    volatile UINT _ticsFromLastTransmittedByte = 0;
    UINT _bytesPerPacket = 10;
    UINT _rxBufferPosition=0;
    UINT _rxBuffer[10];
    bool _txDataAvaliable = false;
    BYTE *_txBuffer;
    UINT _txBufferPosition = 0;
    bool _autoRestore = false;
    bool _transmitting = false;
    uint32_t _fails = 0;
    bool _packetTransmitted = false;
    
    SerialDriver _serial;
    void _SetupSerial();
    void _WaitingForHandshakeState();
    void _HandshakeState();
    void _ActiveState();

void _CheckTxLineState();

void _CheckRxLineState();

    void _BrokenState();
    void _ClearData();
};


Status InitSerialCommunication();

ConnectionStatus GetSerialStatus();

#endif
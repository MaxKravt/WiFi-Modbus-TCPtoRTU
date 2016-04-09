
#ifndef MBTCPSLAVE_H
#define MBTCPSLAVE_H

#include "Arduino.h"
#include <ESP8266WiFi.h>
//#include "RtuMaster.h"


#define MODBUSIP_MAXFRAME   200
#define TCP_BUFFER_SIZE     300
#define FRAME_COUNT         10
#define NO_CLIENT           255
#define MODBUSIP_PORT       502
#define RTU_TIMEOUT        5000

class cTcpSlave
{
  public:
  typedef enum eFrameStatus
  { empty,
    readyToSendRtu,
    waitFromRtu,
    readyToSendTcp
  } frameStatus;

  struct smbFrame
  {
    eFrameStatus status;
    uint8_t nClient;
    uint8_t buffer[TCP_BUFFER_SIZE];
    uint16_t len;
    uint32_t millis;                // Время отправки пакета в Serial
  };

  struct
  {
    WiFiClient client;
    bool onLine;
  } clientOnLine[4];
  struct smbFrame mbFrame[FRAME_COUNT];

  struct smbap
  {
    uint16_t  _ti;  // Transaction Identifier
    uint16_t  _pi;  // Protocol Identifier = 0
    uint16_t  _len; // Length
    uint8_t   _ui;  // Unit Identifier
  };
void waitNewClient (void);  //
void readDataClient(void);  // сканирование клиента на наличие данных
void readFrameClient(WiFiClient  client, uint8_t i);   // разбор данных с клиента
void writeFrameClient(void);
void mbapUnpack (smbap* pmbap, uint8_t * buff );
public:
  cTcpSlave();
  ~cTcpSlave();
  void task(void);
  smbFrame * getFreeBuffer (void);
  smbFrame * getReadyToSendRtuBuffer (void);
  smbFrame * getWaitFromRtuBuffer (void);
  smbFrame * getReadyToSendTcpBuffer (void);

};

extern cTcpSlave TcpSlave;

#endif


#include <ESP8266WiFi.h>

#include "mbTCPslave.h"
#include "RtuMaster.h"
#include "trace.h"
#include "init.h"

#define SERIAL_BAUD        115200

#define MODBUSIP_TIMEOUT   10

void setup()
{
  initESP();
  trace.begin();
  Serial.begin(115200);
}

void loop() {
  
  //trace.println("Cycl OK");
  TcpSlave.task();
  yield();
  trace.task();
  yield();
  _RtuMaster.task();
}

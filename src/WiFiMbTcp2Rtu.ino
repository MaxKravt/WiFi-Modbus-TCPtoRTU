
#include <ESP8266WiFi.h>

#include "h\mbTCPslave.h"
#include "h\RtuMaster.h"
#include "h\trace.h"
#include "h\init.h"

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

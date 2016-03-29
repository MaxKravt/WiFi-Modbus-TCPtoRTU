#include "init.h"
#include <ESP8266WiFi.h>
#include "mbTCPslave.h"
#include "trace.h"


const char* ssid = "KGPA";
const char* password = "Kgpa1972";
const char* hostnameESP = "mbTcp2Rtu";

void WiFiEvent(WiFiEvent_t event);

int statusWifi = 0;

void initESP (void)
{
   WiFi.disconnect(true);
   WiFi.onEvent(WiFiEvent);
   WiFi.hostname(hostnameESP);
   WiFi.begin(ssid, password);
   uint16_t i = 0;
   while(statusWifi)
   {
      i++;

      if (i > 100) {ESP.restart();  i = 0;}
      delay(100);
   }
}

void WiFiEvent(WiFiEvent_t event) {

    switch(event) {
        case WIFI_EVENT_STAMODE_GOT_IP:
            statusWifi = 1;
            break;
        case WIFI_EVENT_STAMODE_DISCONNECTED:
            ESP.wdtDisable();
            delay(1000);
            ESP.restart();
            break;
    }
}

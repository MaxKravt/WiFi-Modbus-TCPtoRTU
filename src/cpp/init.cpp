#include "..\h\init.h"
#include <ESP8266WiFi.h>
#include "..\h\mbTCPslave.h"
#include "..\h\trace.h"
#include "FS.h"


const char* ap_default_ssid = "esp8266"; ///< Default SSID.
const char* ap_default_psk = "esp8266esp8266"; ///< Default PSK.

const char* ssid;// = "KGPA";
const char* pass;// = "Kgpa1972";
const char* hostnameEsp = "mbTcp2Rtu";
IPAddress apIP(192, 168, 1, 1);


void WiFiEvent(WiFiEvent_t event);

int statusWifi = 0;

/******************************************************************************/
/* События     */
void initESP (void)
{
  String station_ssid = "";
  String station_psk = "";
  delay(100);

  SPIFFS.begin();
  if (! loadConfig(&station_ssid, &station_psk))
    {
      station_ssid = "";
      station_psk = "";

      Serial.println("No WiFi connection information available.");
    }

  if (WiFi.getMode() != WIFI_STA)
    {
      WiFi.mode(WIFI_STA);
      delay(10);
    }
    // ... Compare file config with sdk config.
    if (WiFi.SSID() != station_ssid || WiFi.psk() != station_psk)
    {
      // ... Try to connect to WiFi station.
      WiFi.begin(station_ssid.c_str(), station_psk.c_str());

      // ... Pritn new SSID
//      Serial.print("new SSID: ");
//      Serial.println(WiFi.SSID());

      // ... Uncomment this for debugging output.
      //WiFi.printDiag(Serial);
    }
    else
    {
      // ... Begin with sdk config.
      WiFi.begin();
    }

    //Serial.println("Wait for WiFi connection.");

    // ... Give ESP 10 seconds to connect to station.
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000)
    {
      Serial.write('.');
      //Serial.print(WiFi.status());
      delay(500);
    }
//    Serial.println();

    // Check connection
    if(WiFi.status() == WL_CONNECTED)
    {
      // ... print IP Address
//      Serial.print("IP address: ");
//      Serial.println(WiFi.localIP());
    }
    else
    {
//      Serial.println("Can not connect to WiFi station. Go into AP mode.");

      // Go into software AP mode.
      WiFi.mode(WIFI_AP);

      delay(10);
      WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
      WiFi.softAP(ap_default_ssid, ap_default_psk);

//      Serial.print("IP address: ");
//      Serial.println(WiFi.softAPIP());
    }
}
/******************************************************************************/
/* События     */
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
/******************************************************************************/
/* Чтение сида пида     */

bool loadConfig(String *ssid, String *pass)
{
  // open file for reading.
  File configFile = SPIFFS.open("/cl_conf.txt", "r");
  if (!configFile)
  {
    Serial.println("Failed to open cl_conf.txt.");

    return false;
  }

  // Read content from config file.
  String content = configFile.readString();
  configFile.close();

  content.trim();

  // Check if ther is a second line available.
  int8_t pos = content.indexOf("\r\n");
  uint8_t le = 2;
  // check for linux and mac line ending.
  if (pos == -1)
  {
    le = 1;
    pos = content.indexOf("\n");
    if (pos == -1)
    {
      pos = content.indexOf("\r");
    }
  }

  // If there is no second line: Some information is missing.
  if (pos == -1)
  {
    Serial.println("Infvalid content.");
    Serial.println(content);

    return false;
  }

  // Store SSID and PSK into string vars.
  *ssid = content.substring(0, pos);
  *pass = content.substring(pos + le);

  ssid->trim();
  pass->trim();

  return true;
} // loadConfig

/******************************************************************************/
/* Сохранение сида пида     */
bool saveConfig(String *ssid, String *pass)
{
  // Open config file for writing.
  File configFile = SPIFFS.open("/cl_conf.txt", "w");
  if (!configFile)
  {
    Serial.println("Failed to open cl_conf.txt for writing");

    return false;
  }

  // Save SSID and PSK.
  configFile.println(*ssid);
  configFile.println(*pass);

  configFile.close();

  return true;
} // saveConfig

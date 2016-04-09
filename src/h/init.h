#include <ESP8266WiFi.h>


// инициализация платы ESP-01
void initESP (void);
bool loadConfig(String *ssid, String *pass);
bool saveConfig(String *ssid, String *pass);

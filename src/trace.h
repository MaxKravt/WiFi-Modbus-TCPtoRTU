
#ifndef TRACE_H
#define TRACE_H

#include <inttypes.h>
#include "Stream.h"
#include "Arduino.h"
#include <ESP8266WiFi.h>

class cTcpTrace : public Stream
{
  private:
  WiFiClient tcpTraceClient;

  public:
    void begin();
    void task(void);
    virtual int available(void);
    virtual void flush(void);
    virtual int peek(void);
    virtual int read(void);
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buf, size_t size);
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool() { return true; }

};
extern cTcpTrace trace;
#endif

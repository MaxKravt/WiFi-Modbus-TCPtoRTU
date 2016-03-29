#include "trace.h"

WiFiServer tcpTraceServer(23);

/******************************************************************************/
void cTcpTrace::begin()
{
  tcpTraceServer.begin();
}
/******************************************************************************/
void cTcpTrace::task(void)
{
  if (tcpTraceServer.hasClient())
  {
    if (!tcpTraceClient || !tcpTraceClient.connected())
    {
      if(tcpTraceClient) tcpTraceClient.stop();
      tcpTraceClient = tcpTraceServer.available();
      println("Welcom to trace.");
    }
  }
}
/******************************************************************************/
size_t cTcpTrace::write(uint8_t c)
{
  if (tcpTraceClient)
  {
     return tcpTraceClient.write(c);
  }
  return 0;
}
/******************************************************************************/
size_t  cTcpTrace::write(const uint8_t *buf, size_t size)
{
  if (tcpTraceClient)
  {
     return tcpTraceClient.write(buf, size);
     tcpTraceClient.flush();
  }
  return 0;
}
/******************************************************************************/
int cTcpTrace::available(void)
{
  if (tcpTraceClient)
  {
    tcpTraceClient.available();
  }
  else
    return 0;
}
/******************************************************************************/
int cTcpTrace::peek(void)
{
  if (tcpTraceClient)
  {
    return tcpTraceClient.peek();
  }
  else
    return 0;
}

/******************************************************************************/
int cTcpTrace::read(void)
{
  if (tcpTraceClient)
  {
    return tcpTraceClient.read();
  }
  else
    return 0;
}
/******************************************************************************/
void cTcpTrace::flush()
{
  if (tcpTraceClient)
  {
    tcpTraceClient.flush();
  }
}
cTcpTrace trace;

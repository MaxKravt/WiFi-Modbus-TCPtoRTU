
#include "..\h\mbTCPslave.h"
#include "..\h\trace.h"
WiFiServer mbServer(MODBUSIP_PORT);

cTcpSlave::cTcpSlave()
{
  mbServer.begin();
  mbServer.setNoDelay(true);
  for (uint8_t i = 0 ; i < FRAME_COUNT; i++)
    mbFrame[i].status = frameStatus::empty;
}

cTcpSlave::~cTcpSlave()
{
}

void cTcpSlave::waitNewClient(void)
{
   // смотри на живы ли старые клиенты если не живой то освобождаем сонект
   for (uint8_t i = 0 ; i < 4; i++)
   {
      if (clientOnLine[i].onLine && !clientOnLine[i].client.connected())
      {
        clientOnLine[i].client.stop();
        clientOnLine[i].onLine = false;
        trace.print ("Client stop: ");
        trace.println (i);
      }
  //    else clientOnLine[i].client.flush();

   }

   if (mbServer.hasClient())
   {
     bool clientReg = true;
     for(uint8_t i = 0 ; i < 4; i++)
     {
       if( !clientOnLine[i].onLine)
       {
           clientOnLine[i].client = mbServer.available();
           clientOnLine[i].onLine = true;
           trace.print ("New client: ");
           trace.println (clientOnLine[i].client.remoteIP().toString());
           break;
       }
     }
     if (!clientReg) // Tесли не нашлось места новому клиенту
     {
       clientOnLine[0].client.stop();
       clientOnLine[0].client = mbServer.available();
     }
   }
}

void cTcpSlave::readDataClient(void)
{
  for(uint8_t i = 0; i < 4; i++)
  {
    if(clientOnLine[i].onLine)
    {
      if(clientOnLine[i].client.available())
      {
        readFrameClient(clientOnLine[i].client, i);
      }
    }
  }
}

void cTcpSlave::readFrameClient(WiFiClient client, uint8_t nClient)
{
  if ((client.available() < TCP_BUFFER_SIZE) && (client.available() > 11))
  {
    size_t len = client.available();
    uint8_t buf[len];
    size_t count = 0;
    while(client.available()) {buf[count] = client.read(); count++; }
    count =0;
    smbap  mbap;
    mbapUnpack(&mbap, &buf[0]);
  //  trace.println(String("Paket in : len data ") + String(len) +
  //  "Len pak " + String(mbap._len) + ", TI " + String(mbap._ti));

    // проверка на склеенные запросы. (мастера бывают запрашивают по 4 запроса)
    while((count < len ) && ((len - count) <= (mbap._len + 6)) && (mbap._pi ==0))
    {
      smbFrame * pmbFrame = getFreeBuffer();
      if(pmbFrame == 0) break; // если нет свободгого буфера то прикращаем разбор
      pmbFrame->nClient = nClient;
      pmbFrame->status = frameStatus::readyToSendRtu;
      pmbFrame->len = mbap._len + 6;
      pmbFrame->millis   = millis();


      for (uint16_t j = 0; j < (pmbFrame->len); j++)
        pmbFrame->buffer[j] = buf[j];
      count +=  pmbFrame->len;
      mbapUnpack(&mbap, &buf[count]);

    }
    //trace.print ("read paket. Num byte: ");
    //trace.println (mbFrame[i].len);
  }
  else
  {
    uint16_t tmp = client.available();
    while(client.available()) client.read();
  }
}

void cTcpSlave::writeFrameClient(void)
{
  smbFrame * pmbFrame = getReadyToSendTcpBuffer();
  if(pmbFrame)
  {
    uint8_t cli = pmbFrame->nClient;
    size_t len = pmbFrame->len;
    clientOnLine[cli].client.write(&pmbFrame->buffer[0], len);
    delay(1);
    //clientOnLine[cli].client.flush();
    pmbFrame->status = frameStatus::empty;
  }
}

void cTcpSlave::task()
{

  waitNewClient();
  yield();
  readDataClient();
  yield();
  writeFrameClient();
  yield();
  // Очистка буфферов
  for(uint8_t i = 0; i < FRAME_COUNT; i++)
  {
    if(mbFrame[i].status != frameStatus::empty )
      if (millis() - mbFrame[i].millis > RTU_TIMEOUT)
      {
        mbFrame[i].status = frameStatus::empty;
        trace.println ("Del pack.");
      }
  }

}

cTcpSlave::smbFrame * cTcpSlave::getFreeBuffer ()
{
  static uint8_t  scanBuff = 0;
  uint8_t scan = 0;
  while (mbFrame[scanBuff].status != frameStatus::empty)
  {
    scanBuff++;
    if(scan >=  FRAME_COUNT) {trace.println ("No Free buffer"); return 0;}
    if (scanBuff >= FRAME_COUNT) scanBuff = 0;
  }
  return &mbFrame[scanBuff];
}

cTcpSlave::smbFrame * cTcpSlave::getReadyToSendRtuBuffer ()
{
  uint8_t pointer = 255;
  uint8_t pointerMillis = 0;
  for(uint8_t i = 0; i < FRAME_COUNT; i++)
  {
    if(mbFrame[i].status == frameStatus::readyToSendRtu )
    {
      if ( pointerMillis  < (millis() - mbFrame[i].millis))
      {
        pointerMillis = millis() - mbFrame[i].millis;
        pointer = i;
      }
    }
  }
  if (pointer != 255)
    return &mbFrame[pointer];
  else
    return 0;
}

cTcpSlave::smbFrame * cTcpSlave::getWaitFromRtuBuffer ()
{
  for(uint8_t i = 0; i < FRAME_COUNT; i++)
  {
    if(mbFrame[i].status == frameStatus::waitFromRtu )
      return &mbFrame[i];
  }
  return 0;
}
cTcpSlave::smbFrame *cTcpSlave:: getReadyToSendTcpBuffer ()
{
  for(uint8_t i = 0; i < FRAME_COUNT; i++)
  {
    if(mbFrame[i].status == frameStatus::readyToSendTcp )
      return &mbFrame[i];
  }
  return 0;
}
void cTcpSlave::mbapUnpack(smbap* pmbap, uint8_t * buff )
{
  pmbap->_ti  = *(buff + 0) << 8 |  *(buff + 1);
  pmbap->_pi  = *(buff + 2) << 8 |  *(buff + 3);
  pmbap->_len = *(buff + 4) << 8 |  *(buff + 5);
  pmbap->_ui  = *(buff + 6);
}

cTcpSlave TcpSlave;

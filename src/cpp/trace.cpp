#include "..\h\trace.h"

WiFiServer tcpTraceServer(23);

/******************************************************************************/
void cTcpTrace::begin()
{
  tcpTraceServer.begin();
  menuState = 0;
}
/******************************************************************************/
void cTcpTrace::task(void)
{
  if (tcpTraceServer.hasClient())
  {
    //if (!tcpTraceClient || !tcpTraceClient.connected())
    {
      if(tcpTraceClient) tcpTraceClient.stop();
      tcpTraceClient = tcpTraceServer.available();
      println("Welcom to trace.");
    }
  }
  if (tcpTraceClient)
  {
     menu();
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
    return tcpTraceClient.available();
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

/******************************************************************************/
void cTcpTrace::menu()
{
  static String dataIn = "";
  static String dataInChar = "";
  static String station_ssid = "";
  static String station_psk = "";

  switch (menuState)
  {
    case 0:
    {
      if (! loadConfig(&station_ssid, &station_psk))
        {
          station_ssid = "KGPA";
          station_psk = "Kgpa1972";
          saveConfig(&station_ssid, &station_psk);
          println("Save Config");
        }
        menuState=1;
        break;
      }
      case 1:
      {
      println(String()+ "********************************************************************************");
      println(String()+ "* 1. My IP adress: "+    String( WiFi.localIP()) );
      println(String()+ "* 2. My SSID:      "+    station_ssid );
      println(String()+ "* 3. My PSK:       "+    station_psk  );
      println(String()+ "* 0. Save config!   " );

      menuState = 2;
      break;
    }
    case 2:
    {
      while( available())
        {
          dataInChar = readString();
          print(dataInChar);
          dataIn += dataInChar;
        }
        int16_t len = dataIn.indexOf ("\r\n");
        if (len != -1)
        {
          println("");
          uint16_t comm = dataIn.charAt(0);
          switch (comm)
          {
            case 0x31: { menuState = 10; break;}
            case 0x32: { menuState = 20; print("Enter SSID: "); break; }
            case 0x33: { menuState = 30; print("Enter PSK: "); break;}
            case 0x30:
            { menuState = 0;
              print("Saving config.... ");
              saveConfig(&station_ssid, &station_psk);
              break;
            }

            default:
            println("No command!!!" );
            println(comm, HEX);

          }
          dataIn = "";
        }
      break;
    }
    case 10:
    {
      println("Good!!!" );
      menuState = 1;
      break;
    }

    case 20:
    {
      while( available())
      {
        dataInChar = readString();
        print(dataInChar);
        dataIn += dataInChar;
      }
      int16_t len = dataIn.indexOf ("\r\n");
      if (len != -1)
      {
        station_ssid = dataIn.substring(0, len);
        dataIn = "";
        menuState = 1;
      }
      break;
    }

    case 30:
    {
      while( available())
      {
        dataInChar = readString();
        print(dataInChar);
        dataIn += dataInChar;
      }
      int16_t len = dataIn.indexOf ("\r\n");
      if (len != -1)
      {
        station_psk = dataIn.substring(0, len);
        dataIn = "";
        menuState = 1;
      }
      break;
    }



    default:
      menuState = 0 ;
  }


}






cTcpTrace trace;



#include <drivers/XBeeConf.h>

char XBeeConf::read(uint16_t timeout)
{
  uint16_t tmpTimer = millis();
  while (true)
  {
    if (Serial.available())
    {   
      return Serial.read();
    }   
    if (millis()-tmpTimer >= timeout)
    {   
      break;
    }   
  }
  return (byte)13;
}

char XBeeConf::read()
{
  return read(3000);
}

String XBeeConf::readLine()
{
  String data = "";
  char lastRead;
  while(true)
  {
    lastRead = read();
    data = data + lastRead;
    if (lastRead == 13)
      break;
  }
  return data;
}

bool XBeeConf::isOk()
{
  return (read() == 'O' && read() == 'K' && read() == 13);
}

void XBeeConf::send(String command)
{
    Serial.print(command);
    if (command != "+++")
      Serial.print((byte)13);
}

bool XBeeConf::setParam(String command, String value)
{
    send(command+value);
    return isOk();
}

String XBeeConf::getParam(String param)
{
    send(param);
    return readLine();
}

void XBeeConf::setDefaults()
{
  delay(1000);
  bool inAPIMode = setParam("+++", "");

  if (!inAPIMode)
  {
    // In case the RollingPin was restarted 
    // but not booted.
    Serial.begin(57600);
    delay(1000);
    inAPIMode = setParam("+++", ""); 
  }

  if (inAPIMode)
  {
    setParam("ATRE", "");
    setParam("ATID", "3332");
    setParam("ATCH", "B");
    setParam("ATDH", "0x0");
    setParam("ATDL", "0x00000000FFFF");
    setParam("ATMY", "0x00000000FFFF");
    setParam("ATBD", "0x06");
    setParam("ATAP", "2");
    setParam("ATWR", "");

    setParam("ATCN", "");
    Serial.begin(57600);
  }
}

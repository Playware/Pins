

#include "XBeeAPIConf.h"


void XBeeAPIConf::setup()
{
  //atRequest = AtCommandRequest();
}

bool XBeeAPIConf::setParam(char* cmd, char* value, uint8_t length)
{
  atRequest.setCommand((uint8_t*)cmd);
  if (value != NULL)
  {
    atRequest.setCommandValue((uint8_t*)value);
  }
  atRequest.setCommandValueLength(length);

  send();
  atRequest.clearCommandValue();
  xbee.readPacket(1000);
  if(xbee.getResponse().isAvailable())
  {
    if(xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE)
    {   
      xbee.getResponse().getAtCommandResponse(atResponse);
      return atResponse.isOk();
    }   
  }
  return false;
}

void XBeeAPIConf::send()
{
  xbee.send(atRequest);
  if(xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE)
  {
    xbee.getResponse().getAtCommandResponse(atResponse);
  }
}

void XBeeAPIConf::setDefaults()
{
  char cmd;
  char value;
  uint16_t value16;
  //setParam("CH", &value, 1);
  //setParam("RE", NULL, 0);
  value = 0x0B;
  setParam("CH", &value, 1);
  value = 0x0;
  setParam("DH", &value, 1);
  value16 = 0x0001;
  setParam("DL", (char*)&value16, 2);
  setParam("MY", (char*)&value16, 2);
  value = 0x02;
  //setParam("AP", (char*)&value, 1);

}

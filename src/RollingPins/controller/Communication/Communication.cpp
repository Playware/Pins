
#include "Communication.h"
#include "Frame.h"
#include <drivers/XBeeConf.h>
#include "controller/EventManager/EventManager.h"

XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x0000FFFF);

char EVENT_MESSAGE_RECEIVED[] = "XV";

void (*packetListener_)(Message_t*) = NULL;

void COM_act(char* eventName, void* event)
{
  comm.receiveAndReact();
}

void Communication::setup()
{
  eventManager.registerType(EVENT_MESSAGE_RECEIVED);
};

void Communication::receiveAndReact()
{
  reactOnMessage(receive());
};

Message_t* Communication::receive()
{
  xbee.readPacket();
  return processMessage_();
}

Message_t* Communication::receive(int timeout)
{
  xbee.readPacket(timeout);
  return processMessage_();
}

Message_t* Communication::processMessage_()
{
  if(xbee.getResponse().isAvailable())
  {
    if(xbee.getResponse().getApiId() == RX_64_RESPONSE)
    {
      xbee.getResponse().getRx64Response(rx);
      // Get starting pointer to the actual data
      uint8_t* tmpAddr = rx.getFrameData() + rx.getDataOffset();
      message.data = rx.getFrameData() + rx.getDataOffset() + FRAME_HEADER;
      message.data_length = rx.getDataLength();
      return &message;
    }
    else if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE)
    {
      xbee.getResponse().getZBTxStatusResponse(txStatus);
      /*if (txStatus.getStatus() != SUCCESS)
      {
        // Do something?
      }*/
    }
  }
  return NULL;
}

void Communication::reactOnMessage(Message_t* message)
{
  if (message == NULL)
    return;
	#ifndef TILES_COMPATABLE
  	react((char*)&message->data[0], message); 
	#else
  	react((char*)&message->data[1], message); 
	#endif
}

void Communication::send(uint8_t* data, uint8_t length)
{
#ifdef TILES_COMPATABLE
  data[0] = 0x7e;
#endif

	for (int i = 0; i<length; i++)
  {
    buffer[i+FRAME_HEADER] = data[i];
  }
  tx = Tx64Request(addr64, buffer, FRAME_HEADER + length);
  xbee.send(tx);
  reactOnMessage(receive(1000));
};

void Communication::send(const char* str)
{
  uint8_t i = 0;
  while (*str++)
    i++;
  str = str-i-1;
  send((uint8_t*)str, i);
}

void Communication::subscribe(char &msgType, void(*msgHandler)(Message_t*))
{
  MsgSubscription* ptr = (MsgSubscription*)malloc(sizeof(MsgSubscription));
  ptr->type = &msgType;
  ptr->handler = msgHandler;

  if (subscriptions == NULL)
  {
    subscriptions = LinkedList::createNode(ptr);
  }
  else
  {
    LinkedList::insertAfter(subscriptions, ptr);
  }
}

void Communication::react(char* msgType, Message_t* message)
{
  if(subscriptions != NULL)
  {
    ListNode* subscrNode = subscriptions;
    MsgSubscription* subscr;
    while(subscrNode != NULL)
    {
      subscr = (MsgSubscription*)subscrNode->data;

      if (*(subscr->type) == *msgType)
      {
        subscr->handler(message);
      }

      subscrNode = subscrNode->next;
    }
  }
}

void Communication::removeAllSubscriptions()
{
  ListNode* node = subscriptions;
  while(node != NULL)
  {
    free((MsgSubscription*)node->data);
    LinkedList::removeNode(&subscriptions, NULL, node);
    node = subscriptions;
  }

}

Communication comm;

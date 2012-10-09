/*
 * Communication via XBee, using API mode (2)
 *
 */

#ifndef Communication_h
#define Communication_h

//#include "Arduino.h"
#include "/usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h"
#include "libraries/XBee/XBee.h"
#include "Frame.h"
#include "controller/LinkedList/LinkedList.h"

#define MAX_DATA_LENGTH 25

extern char EVENT_MESSAGE_RECEIVED[];

enum CommandFlags
{
  TO_APPLICATION,
  SELECT_APPLICATION,
  RESET
};

struct MsgSubscription
{
  char* type;
  void (*handler)(Message_t* data);
};

void COM_act(char* eventType, void* event);

class Communication
{
  ListNode* subscriptions;
  XBee xbee;
  Message_t message;
  Tx64Request tx;
  Rx64Response rx;
  TxStatusResponse txStatus;
  uint8_t buffer[MAX_DATA_LENGTH];

  public:
    void setup();
    Message_t* receive();
    Message_t* receive(int timeout);
    void reactOnMessage(Message_t* message);
    void receiveAndReact();
    void send(uint8_t* value, uint8_t size);
    void send(const char* str);
    void send(Message_t* message);
    void subscribe(char &msgType, void (*msgHandler)(Message_t*));
    void removeAllSubscriptions();

  private:
    Message_t* processMessage_();
    void react(char* msgType, Message_t* message);
};

extern Communication comm;

#endif

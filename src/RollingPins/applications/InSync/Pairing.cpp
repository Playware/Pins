
#include "Pairing.h"

/*
 * Broadcast request pairing
 * Receive request for pairing, send handshake
 *   onPaired
 * Receive handshake, mark as master
 *   onPaired
 *
 */

enum Msg_type {TYPE_REQUEST, TYPE_HANDSHAKE};

struct Pair_msg
{
  uint8_t msgType;
  uint8_t type;
} pairMsg;

static char MSG_PAIR = 10;

PairingManager_t* pairingManager = NULL;

PairingManager_t* Pairing::start(void (*onPair)(void))
{
  RP_idleStart();
  if (pairingManager != NULL)
    free(pairingManager);
  else
    comm.subscribe(MSG_PAIR, handleMessage);

  pairMsg.msgType = MSG_PAIR;
  pairingManager = (PairingManager_t*)malloc(sizeof(struct PairingManager_t));
  pairingManager->onPair = onPair;
  pairingManager->isConnected = false;
  pairingManager->isMaster = false;

  pairingManager->requestTimer = timerManager.createPeriodic(0, 500, broadcastRequest);
  return pairingManager;
}

void Pairing::broadcastRequest()
{
  pairMsg.type = TYPE_REQUEST;
  comm.send((uint8_t*)&pairMsg, sizeof(struct Pair_msg));
}

void Pairing::handleMessage(Message_t* msg)
{
  if (pairingManager != NULL && !pairingManager->isConnected)
  {
    Pair_msg* pair = (Pair_msg*)msg->data;
    if (pairingManager != NULL)
    {
      switch(pair->type)
      {
        case TYPE_REQUEST:
          timerManager.remove(&(pairingManager->requestTimer));
          pairMsg.type = TYPE_HANDSHAKE;
          comm.send((uint8_t*)&pairMsg, sizeof(struct Pair_msg));
          pairingManager->isMaster = false;
          onPaired();
          break;
        case TYPE_HANDSHAKE:
          timerManager.remove(&(pairingManager->requestTimer));
          pairingManager->isMaster = true;
          onPaired();
          light.setAll(LIGHT_GREEN);
          light.update();
          delay(500);
          light.clearAll();
          light.update();
         break;
      }
    }
  }
}

void Pairing::onPaired()
{
  pairingManager->isConnected = true;
  if (pairingManager->onPair != NULL)
    pairingManager->onPair();
  RP_idleStop();
}

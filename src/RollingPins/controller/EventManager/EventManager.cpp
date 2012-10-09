
#include "EventManager.h"

EventManager::EventManager()
{
  events = NULL;
  subscriptions = NULL;
}

void EventManager::setup()
{
}

uint8_t EventManager::registerType(char* eventType)
{
  Event_t* ptr = (Event_t*)malloc(sizeof(Event_t));
  ptr->eventType = eventType;
  if (events == NULL)
  {
    events = LinkedList::createNode(ptr);
  }
  else
  {
    LinkedList::insertAfter(events, ptr);
  }
}

void EventManager::unregisterType(char* eventType)
{
  // Not implemented yet!
}

void EventManager::subscribeListener(char* eventType, void (*listener)(char* eventType, void* event))
{
  EventSubscription_t* ptr = (EventSubscription_t*)malloc(sizeof(EventSubscription_t));
  ptr->eventType = eventType;
  ptr->listener = listener;

  if (subscriptions == NULL)
  {
    subscriptions = LinkedList::createNode(ptr);
  }
  else
  {
    LinkedList::insertAfter(subscriptions, ptr);
  }
}

void EventManager::unsubscribeListener(char* eventType, void (*listener)(char* eventType, void* event))
{
  // Not implemented yet!
}

void EventManager::triggerEvent(char* eventType, void* event)
{
  if (subscriptions != NULL)
  {
    ListNode* subscrNode = subscriptions;
    EventSubscription_t* subscr;
    while(subscrNode != NULL)
    {
      subscr = (EventSubscription_t*)subscrNode->data;
      
      if (subscr->eventType == eventType)
      {
        subscr->listener(eventType, event);
      }

      subscrNode = subscrNode->next;
    }
  }
}

void EventManager::removeAllSubscriptions()
{
  ListNode* node = subscriptions;
  while(node != NULL)
  {
    free((EventSubscription_t*)node->data);
    LinkedList::removeNode(&subscriptions, NULL, node);
    node = subscriptions;
  }
}

EventManager eventManager;

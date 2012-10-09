
#include "TimerManager.h"

void TIM_act(char* eventType, void* event)
{
  timerManager.act();
}

Timer_t* TimerManager::createPeriodic(unsigned long start, unsigned long interval, void (*callback)())
{
  return createNShot(start, interval, -1, callback);
}

Timer_t* TimerManager::createOneShot(unsigned long start, void (*callback)())
{
  return createNShot(start, 0, 1, callback);
}

Timer_t* TimerManager::createNShot(unsigned long start, unsigned long interval, int shots, void (*callback)())
{
  Timer_t* timer = (Timer_t*)malloc(sizeof(Timer_t));
  timer->callback = callback;
  timer->isPeriodic = shots < 0 ? true : false;
  timer->shotsLeft = shots;
  timer->nextShot = start + millis();
  timer->interval = interval;

  if (timers == NULL)
  {
    timers = LinkedList::createNode(timer);
  }
  else
  {
    LinkedList::insertAfter(timers, timer);
  }
  return timer;
}

void TimerManager::act()
{
  ListNode* node = timers;
  ListNode* prevNode = NULL;
  Timer_t* timer;
  long currTime = millis();
  while(node != NULL)
  {
    timer = (Timer_t*)node->data;

    if (currTime > timer->nextShot)
    {
      timer->callback();
      timer->nextShot += timer->interval;
      if (!timer->isPeriodic)
      {
        timer->shotsLeft--;
        if (timer->shotsLeft == 0)
        {
          free(timer);
          LinkedList::removeNode(&timers, prevNode, node);
          node = prevNode;
        }
      }
    }
    prevNode = node; 
    node = node->next;
  }
}

void TimerManager::remove(Timer_t** timer)
{
  ListNode* node = timers;
  ListNode* prevNode = NULL;
  while(node != NULL)
  {
    if (node->data == *timer)
    {
      free(*timer);
      LinkedList::removeNode(&timers, prevNode, node);
      *timer = NULL;
      break;
    }
    prevNode = node;
    node = node->next;
  }
}

void TimerManager::removeAll()
{
  ListNode* node = timers;
  while(node != NULL)
  {
    free((Timer_t*)node->data);
    LinkedList::removeNode(&timers, NULL, node);
    node = timers;
  }
}

TimerManager timerManager;

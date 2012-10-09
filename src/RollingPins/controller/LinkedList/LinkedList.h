#ifndef LinkedList_h
#define LinkedList_h

//#include "Arduino.h"
#include "/usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h"

struct ListNode {
  void* data;
  ListNode* next;
};

namespace LinkedList
{
  ListNode* createNode(void* data);
  uint16_t getSize(ListNode* node);
  ListNode* insertAfter(ListNode* node, void* data);
  void removeNodeFromList(ListNode** startNode, ListNode* nodeToRemove);
  void removeNode(ListNode** startNode, ListNode* prevNode, ListNode* nodeToRemove);
};

#endif

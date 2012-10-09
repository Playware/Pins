
#include "LinkedList.h"

ListNode* LinkedList::createNode(void* data)
{
  ListNode* node = (ListNode*)malloc(sizeof(ListNode));
  node->data = data;
  node->next = NULL;
  return node;
}

uint16_t LinkedList::getSize(ListNode* node)
{
  int16_t size = 0;
  while (node->next != NULL)
  {
    size++;
    node = node->next;
  }
  return size;
}

ListNode* LinkedList::insertAfter(ListNode* node, void* data)
{
  ListNode* newNode = createNode(data);
  if (node->next != NULL)
  {
    newNode->next = node->next;
  }
  node->next = newNode;
  return newNode;
}

/* NOTE: Not tested */
void LinkedList::removeNodeFromList(ListNode** startNode, ListNode* nodeToRemove)
{
  if (*startNode == NULL || nodeToRemove == NULL)
  {
    return;
  }
  ListNode* prevNode = NULL;
  ListNode* node = *startNode;
  while(1)
  {
    if (node == nodeToRemove)
    {
      removeNode(startNode, prevNode, node);
      break;
    }

    if (node->next == NULL)
    {
      break;
    }
    prevNode = node;
    node = node->next;
  }
}

void LinkedList::removeNode(ListNode** startNode, ListNode* prevNode, ListNode* nodeToRemove)
{
  if (prevNode != NULL)
  {
    prevNode->next = nodeToRemove->next;
  }
  else if (*startNode == nodeToRemove)
  {
    *startNode = nodeToRemove->next;
  }
 
  free(nodeToRemove);
  nodeToRemove = NULL;
}

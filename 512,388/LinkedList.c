#include "LinkedList.h"

// Just initializes the list structure (the node pointers are NULL and the
// count = 0).
List *listCreate()
{
  List *new = malloc(sizeof(*new));
  new->count = 0;
  new->first = NULL;
  new->last = NULL;
  return new;
}

// Just initializes a ListNode structure (the node pointers are NULL and the
// pointer to the data is set to payload).
ListNode *list_create_node(void *payload)
{
  ListNode *new = malloc(sizeof(ListNode));
  if (new != NULL)
  {
      new->next = NULL;
      new->prev = NULL;
      new->value = payload;
  }
  return new;
}

// Frees any nodes in the list and then frees the list structure.
List *list_destroy(List *list)
{
  list_clear(list);
  free(list);
  return NULL;
}

// Frees any nodes in the list but leaves the list structure.
void list_clear(List *list)
{
  ListNode *cur = list->first;
  ListNode *next;
  while(cur != NULL)
  {
    next = cur->next;
    free(cur);
    cur = next;
  }
  list->first = NULL;
  list->last = NULL;
  list->count = 0;
}

// Returns the count in the list structure.
int list_count(List *list)
{
  return list->count;
}

// Returns the first node in the list.
ListNode *listFirst(List *list)
{
  return list->first;
}

// Returns the last node in the list.
ListNode *list_last(List *list)
{
  return list->last;
}

// Finds a node in the list by value and returns the pointer to the node. If no
// matching node is found, returns NULL.
ListNode *list_find(List *list, void *value)
{
  ListNode *cur = list->first;
  while(cur != NULL)
  {
    if(cur->value == value)
    {
      return cur;
    }
    cur = cur->next;
  }
  return NULL;
}

// Inserts a node in the list after the node containing value.  If no node has
// the passed value or if value == NULL, insert at the end of the list.
void list_insert_after(List *list, ListNode *node, void *value)
{
  ListNode *insert_after = list_find(list, value);
  if(list->first == NULL)
  {
    list->first = node;
  }
  else if(insert_after == NULL || value == NULL)
  {
    list->last->next = node;
    node->prev = list->last;
  }
  else
  {
    node->prev = insert_after;
    node->next = insert_after->next;
    insert_after->next = node;
  }
  node->next != NULL ? (node->next->prev = node) : (list->last = node);
  list->count++;
}

// Inserts a node in the list before the node containing value.  If no node has
// the passed value or if value == NULL, insert at the beginning of the list.
void list_insert_before(List *list, ListNode *node, void *value)
{
  ListNode *insert_before = list_find(list, value);
  if(list->last == NULL)
  {
    list->last = node;
  }
  else if(insert_before == NULL || value == NULL)
  {
    list->first->prev = node;
    node->next = list->first;
  }
  else
  {
    node->next = insert_before;
    node->prev = insert_before->prev;
    insert_before->prev = node;
  }
  node->prev != NULL ? (node->prev->next = node) : (list->first = node);
  list->count++;
}

// Removes the specified node from the list and frees it.  The node's value is saved and returned.
void *listRemoveNode(List *list, ListNode* node)
{
  void *ret = node->value;
  if(node == list->first)
  {
    list->first = node->next;
  }

  if(node == list->last)
  {
    list->last = node->prev;
  }

  if(node->prev != NULL)
  {
    node->prev->next = node->next;
  }

  if(node->next != NULL)
  {
    node->next->prev = node->prev;
  }
  free(node);
  list->count--;
  return ret;
}

// Removes the node with the specified value from the list.  The node's value is
// saved and returned.  If a node with the value is not found in the list,
// return NULL.
void* listRemoveValue(List* list, void* value)
{
  ListNode *ln = list_find(list, value);
  if(ln == NULL)
  {
    return NULL;
  }
  return listRemoveNode(list, ln);
}

void listAddFirst(List* list, void* value)
{
    list_insert_before( list, list_create_node(value), NULL );
}

void listAddLast(List* list, void* value)
{
    list_insert_after( list, list_create_node(value), NULL );
}

void* listRemoveFirst(List* list)
{
    return listRemoveNode( list, list->first );
}

void* listRemoveLast(List* list)
{
    return listRemoveNode( list, list->last );
}

/*
  File:        sfs_queue.c
  Description: Queue definitions.
  Author:      Charles R.
*/

#include "sfs_queue.h"
#include <stdlib.h>

/******************************************************************************/
/*                        Public Queue Operations                             */
/*                   All documentation is found in header                     */
/******************************************************************************/

void queue_init(queue* q) {
  q->head = NULL;
  q->length = 0;
}

void queue_destroy(queue* q) {
  if(q->head == NULL) return;
  qnode* current = q->head;
  while(current != NULL) {
    qnode* next = current->next;
    free(current->data);
    free(current);
    current = next;
  }
}

void queue_enqueue(queue* q, void* data) {
  if(q->head == NULL) {
    q->head = (qnode*) malloc(sizeof(qnode));
    q->head->next = NULL;
    q->head->data = data;
  } else {
    qnode* current = q->head;
    while(current->next != NULL)      
      current = current->next;
    current->next = (qnode*) malloc(sizeof(qnode));
    current->next->next = NULL;
    current->next->data = data;
  }
  q->length++;
}

void* queue_dequeue(queue* q) {
  if(q->head == NULL) return NULL;
  qnode* head = q->head;
  void* data = head->data;
  q->head = q->head->next;
  q->length--;
  free(head);
  return data;
}

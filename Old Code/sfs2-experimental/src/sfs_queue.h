/*
  File:        sfs_queue.h
  Description: Queue declarations.
  Author:      Charles R.
*/

#ifndef SFS_QUEUE_H
#define SFS_QUEUE_H

#include <stdint.h>

/** Node structure */
typedef struct qnode {
  struct qnode* next;
  void* data;
} qnode;

/** Queue structure */
typedef struct queue {
  uint32_t length;
  qnode* head;
} queue;

/** 
  Initialize a queue
  Params:  queue - pointer to the queue
*/
void queue_init(queue* q);

/** 
  Destroy a queue
  Params:  queue - pointer to the queue
*/
void queue_destroy(queue* q);

/** 
  Add data to a queue
  Params:  queue - pointer to the queue
           data  - pointer to the data to enqueue
*/
void queue_enqueue(queue* q, void* data);


/** 
  Pop the head off of the queue and get the data
  Params:  queue - pointer to the queue
  Returns: pointer to popped head's data
*/
void* queue_dequeue(queue* q);

#endif

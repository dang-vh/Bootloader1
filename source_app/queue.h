#ifndef _QUEUE_H_
#define _QUEUE_H_
#include <stdint.h>
#define TRUE			(1U)
#define FALSE			(0U)
#define QUEUE_MAX_SIZE 	(10U)

typedef struct {
    int8_t Front, Rear, Size;
    uint32_t *QueueArr; // ptr to Arr
} Queue_Types;

void Queue_PushData(Queue_Types *Queue, uint32_t InputData);





















#endif /* _QUEUE_H_ */

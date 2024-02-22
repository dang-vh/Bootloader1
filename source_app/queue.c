#include "queue.h"
/*!
 * @brief Function to push data into Array
 */
void Queue_PushData(Queue_Types *Queue, uint32_t InputData)
{
    if (Queue->Size >= QUEUE_MAX_SIZE)
    {
        // Overwrite oldest data if queue is full
        if (++Queue->Front >= QUEUE_MAX_SIZE)
        {
            Queue->Front = 0;
        }
        // Do not increase Size because of overwritten
    }
    else
    {
        // Increase Size
        Queue->Size++;
    }

    if (++Queue->Rear >= QUEUE_MAX_SIZE)
    {
        Queue->Rear = 0;
    }

    Queue->QueueArr[Queue->Rear] = InputData;
}

/* Check empty of Queue
uint8_t Queue_IsEmpty(Queue_Types *Queue)
{
    uint8_t Key = FALSE;
    if(Queue->Capacity == QUEUE_EMPTY)
    {
        Key = TRUE;
    }
    return Key;
}

*/
// uint8_t Queue_IsFull (Queue_Types *Queue)
//{
//	uint8_t Key = FALSE;
//	if ((Queue->Front == Queue->Rear + 1) || (Queue->Front == 0 && Queue->Rear == Queue->Size - 1))
//	{
//		Key = TRUE;
//	}
//
//	return Key;
// }

// void Queue_PushData(Queue_Types *Queue, uint32_t InputData)
//{
//	if (!Queue_IsFull(Queue))
//	{
//		if (Queue->Front == -1)
//		{
//		Queue->Front = 0;
//		}
//		Queue->Rear = (Queue->Rear + 1) % Queue->Size;
//		Queue->QueueArr[Queue->Rear] = InputData;
//	}
// }

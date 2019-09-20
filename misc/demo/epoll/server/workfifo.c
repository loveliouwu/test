#include"workfifo.h"


int Create_queue(sequeue_t **pqueue)
{
	sequeue_t *queue;
	queue = (sequeue_t *)malloc(sizeof(sequeue_t));
	if (NULL == queue) return -1;
	
	queue->front = queue->rear = 0;
 
	*pqueue = queue;
    return 0;
}
//销毁队列
void Destroy_queue(sequeue_t *queue)
{
    if(NULL != queue)
    {
        free(queue);
    }
}
//队列是否为空
int isEmpty_queue(sequeue_t *queue)
{
    if(NULL == queue)
        return -1;
    return (queue->front == queue->rear ? 1:0);
}
//对列是否满
int isFull_queue(sequeue_t *queue)
{
    if(NULL == queue)
        return -1;
    return ((queue->rear +1) % N == queue->front ? 1:0);
}
//清空队列
void Clear_queue(sequeue_t *queue)
{
    if(NULL == queue)   return;
    queue->front = queue->rear = 0;
    return;
}
//出队
int De_queue(sequeue_t *queue,int *x)
{
    if(NULL == queue) 
        return -1;
    if(1 == isEmpty_queue(queue)) 
        return -1;
    queue->front = (queue->front + 1) % N;
    if(NULL != x)
    {
        *x = queue->data[queue->front];
    }
    return 0;
}
//入队
int En_queue(sequeue_t *queue,int x)
{
    if(NULL == queue)
        return -1;
    if(1 == isFull_queue(queue))
        return -1;
    queue->rear = (queue->rear + 1) % N;
    queue->data[queue->rear] = x;
    return 0;
}

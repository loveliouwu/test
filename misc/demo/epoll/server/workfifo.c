#include"workfifo.h"


int Create_queue(sequeue_t **pqueue)
{
    pthread_mutex_init(&thread_t,NULL);
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
int De_queue(sequeue_t *queue,data_t *x)
{
    pthread_mutex_lock(&thread_t);
    if(NULL == queue) 
    {
        pthread_mutex_unlock(&thread_t);
        return -1;
    }
    
    if(1 == isEmpty_queue(queue)) 
    {
        printf("queue is empty");
        pthread_mutex_unlock(&thread_t);
        return -1;
    }
        
    queue->front = (queue->front + 1) % N;
    if(NULL != x)
    {
        x->fd = queue->data[queue->front];
        queue->data[queue->front] = 0;
        x->fd_data = queue->data_addr[queue->front];
        queue->data_addr[queue->front] = NULL;
    }
    pthread_mutex_unlock(&thread_t);
    return 0;
}
//入队
int En_queue(sequeue_t *queue,data_t x)
{
    pthread_mutex_lock(&thread_t);
    if(NULL == queue)
    {
        
        pthread_mutex_unlock(&thread_t);
        return -1;
    }
    if(1 == isFull_queue(queue))
    {
        printf("queue is full");
        pthread_mutex_unlock(&thread_t);
        return -1;
    }
    queue->rear = (queue->rear + 1) % N;
    queue->data[queue->rear] = x.fd;
    queue->data_addr[queue->rear] = x.fd_data;
    pthread_mutex_unlock(&thread_t);
    return 0;
}

int Inor_queue(sequeue_t *queue,data_t x)
{
    pthread_mutex_lock(&thread_t);
    int i = queue->front;
    while(queue->data[i] != x.fd)
    {
        if(i == queue->rear)
        {
            pthread_mutex_unlock(&thread_t);
            return 0;
        }
            
        i = (i+1)%N;
    }
    pthread_mutex_unlock(&thread_t);
    return 1;
}

int Show_queue(sequeue_t *queue)
{
    printf("Front: %d:%d  Rear: %d:%d\n",queue->front,queue->data[queue->front],queue->rear,queue->data[queue->rear]);
    return 0;
}


int De_queue_by_fd(sequeue_t *queue,int fd,data_t *data)
{
    pthread_mutex_lock(&thread_t);
    int i = (queue->front + 1) % N;
    int tmp = (queue->front + 1) % N;
    while(i <= queue->rear)
    {
        if(queue->data[i] == fd)
        {
            data->fd = fd;
            data->fd_data = queue->data_addr[i];
            queue->data[i] = queue->data[tmp];
            queue->data_addr[i] = queue->data_addr[tmp];
            queue->data[tmp] = 0;
            queue->data_addr[tmp] = NULL;
            queue->front = (queue->front + 1) % N;
            pthread_mutex_unlock(&thread_t);
            return 0;
        }
        i++;
    }
    pthread_mutex_unlock(&thread_t);
    return -1;
}
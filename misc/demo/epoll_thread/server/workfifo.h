#ifndef _WORKFIFO_H_
#define _WORKFIFO_H_

#include"stdio.h"
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>

pthread_mutex_t thread_t;
#define N 10  //队列中数据元素的数据类型
typedef struct  data
{
    int fd;
    char *fd_data;

}data_t;
typedef struct 
{
	int data[N]; //用数组作为队列的储存空间
    unsigned char *data_addr[N];//数据地址
	int front,rear; //指示队头位置和队尾位置的指针
}sequeue_t;

int Create_queue(sequeue_t **pqueue);
void Destroy_queue(sequeue_t *queue);
int isEmpty_queue(sequeue_t *queue);
int isFull_queue(sequeue_t *queue);
void Clear_queue(sequeue_t *queue);
int De_queue(sequeue_t *queue,data_t *x);
int En_queue(sequeue_t *queue,data_t x);
int Inor_queue(sequeue_t *queue,data_t x);
int Show_queue(sequeue_t *queue);

int De_queue_by_fd(sequeue_t *queue,int fd,data_t *data);

#endif
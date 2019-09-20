#ifndef _WORKFIFO_H_
#define _WORKFIFO_H_

#include"stdio.h"
#include<stdlib.h>
#include<stdio.h>


#define N 64  //队列中数据元素的数据类型
typedef struct  data
{
    int fd;
    char *fd_data;

}data_t;
typedef struct 
{
	int data[N]; //用数组作为队列的储存空间
	int front,rear; //指示队头位置和队尾位置的指针
}sequeue_t;

int Create_queue(sequeue_t **pqueue);
void Destroy_queue(sequeue_t *queue);
int isEmpty_queue(sequeue_t *queue);
int isFull_queue(sequeue_t *queue);
void Clear_queue(sequeue_t *queue);
int De_queue(sequeue_t *queue,int *x);
int En_queue(sequeue_t *queue,int x);



#endif
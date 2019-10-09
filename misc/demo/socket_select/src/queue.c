/*
 * queue.c
 *
 *  Created on: 2019年9月27日
 *      Author: jjn
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "../include/queue.h"

void workQueue(work_queue **p,long size)
{
	if (size < 0)
		size = 128;
	(*p)=(work_queue*)malloc(sizeof(work_queue));
	(*p)->taskPoll =(workStruct*)malloc(sizeof(workStruct) * size);
	(*p)->produce = 0;
	(*p)->consume = 0;
	(*p)->maxSize = size;
	(*p)->workNum = 0;
	pthread_mutex_init(&(*p)->workMutex,NULL);
}

void workQueueQuit(work_queue **p)
{
	if ((*p)->taskPoll != NULL)
		free(NULL);
	pthread_mutex_destroy(&(*p)->workMutex);
}



int getWork(work_queue **p,void** ptr, unsigned long *flag, unsigned long *data)
{
	if ((*p)->taskPoll == NULL)
		return -1;
	int findWork = -1;
	workStruct *q;

	if (pthread_mutex_trylock(&(*p)->workMutex) != 0)
	{
		return -1;
	}
	if ((*p)->workNum > 0)
	{
		q=(*p)->taskPoll+ (*p)->consume;
		*ptr =(void *)q->ptr;
		*flag = q->flag;
		*data =q->data;
		(*p)->consume++;
		(*p)->workNum--;
		//printf("getWork ptr=%#x,workNum=%ld\n", (unsigned long)ptr, workNum);
		if ((*p)->consume >= (*p)->maxSize)
			(*p)->consume = 0;
		findWork = 0;
	}
	pthread_mutex_unlock(&(*p)->workMutex);
	return findWork;
}

int putWork(work_queue **p,void* ptr, unsigned long flag, unsigned long data)
{
	if ((*p)->taskPoll == NULL)
			return -1;
	int ret = -1;
	workStruct *q;
	if (pthread_mutex_trylock(&(*p)->workMutex) != 0)
	{
		return -1;
	}
	if ((*p)->workNum < (*p)->maxSize)
	{
		q=(*p)->taskPoll+ (*p)->consume;
		q->ptr =(unsigned long) ptr;
		q->data = (unsigned long)data;
		q->flag = (unsigned long)flag;
		(*p)->produce++;
		(*p)->workNum++;
		if ((*p)->produce >= (*p)->maxSize)
			(*p)->produce = 0;
		ret = 0;
	}
	pthread_mutex_unlock(&(*p)->workMutex);
	return ret;
}
int getWorkNum(work_queue **p)
{
	return (*p)->workNum;
}


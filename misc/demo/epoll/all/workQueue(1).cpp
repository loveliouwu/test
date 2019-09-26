#include <cstdio>
#include <iostream>
#include "workQueue.h"
using namespace std;


workQueue::workQueue()
{
	taskPoll =(struct workStruct*)malloc(sizeof(struct workStruct) * 128);
	produce = 0;
	consume = 0;
	maxSize = 128;
	workNum = 0;
	pthread_mutex_init(&workMutex,NULL);
}


workQueue::workQueue(long size)
{
	if (size < 0)
		size = 128;
	taskPoll = (struct workStruct *)malloc(sizeof(struct workStruct) * size);
	produce = 0;
	consume = 0;
	workNum = 0;
	maxSize = size;
//	printf("init %#x\n", &workMutex);
	pthread_mutex_init(&workMutex, NULL);
}
workQueue::~workQueue()
{
	if (taskPoll != nullptr)
		free(nullptr);
	pthread_mutex_destroy(&workMutex);
}



bool workQueue:: getWork(void** ptr, unsigned long *flag, unsigned long *data)
{
	if (taskPoll == nullptr)
		return false;
	bool findWork = false;
	
	if (pthread_mutex_trylock(&workMutex) != 0)
	{
		return false;
	}
	if (workNum > 0)
	{
		*ptr =(void *)((taskPoll + consume)->ptr);
		*flag = ((taskPoll + consume)->flag);
		*data = ((taskPoll + consume)->data);
		consume++;
		workNum--;
		//printf("getWork ptr=%#x,workNum=%ld\n", (unsigned long)ptr, workNum);
		if (consume >= maxSize)
			consume = 0;
		findWork = true;

	}
	pthread_mutex_unlock(&workMutex);
	return findWork;
}

bool workQueue::putWork(void* ptr, unsigned long flag, unsigned long data)
{
	if (taskPoll == nullptr)
		return false;
	bool ret = false;
	if (pthread_mutex_trylock(&workMutex) != 0)
	{
		return false;
	}
	if (workNum < maxSize)
	{
		(taskPoll + produce)->ptr =(unsigned long) ptr;
		(taskPoll + produce)->data = (unsigned long)data;
		(taskPoll + produce)->flag = (unsigned long)flag;
		produce++;
		workNum++;
		if (produce >= maxSize)
			produce = 0;
		ret = true;
	}
	pthread_mutex_unlock(&workMutex);
	return ret;
}
int workQueue::getWorkNum()
{
	return workNum;
}
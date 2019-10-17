#ifndef INCLUDE_QUEUE_H_
#define INCLUDE_QUEUE_H_
typedef struct workStruct_
{
	int data;
}workStruct;

typedef struct
{
	unsigned long maxSize ;
	unsigned long workNum ;
	pthread_mutex_t workMutex;
	unsigned long produce;
	unsigned long consume;
	workStruct* taskPoll;
}work_queue;

void workQueue(work_queue **p,long size);
void workQueueQuit(work_queue **p);
int getWork(work_queue **p,int *data);
int putWork(work_queue **p, int data);
int getWorkNum(work_queue **p);
#endif

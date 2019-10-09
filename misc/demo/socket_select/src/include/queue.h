#ifndef INCLUDE_QUEUE_H_
#define INCLUDE_QUEUE_H_
typedef struct workStruct_
{
	unsigned long ptr;
	unsigned long flag;
	unsigned long data;
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
int getWork(work_queue **p,void** ptr, unsigned long *flag, unsigned long *data);
int putWork(work_queue **p,void* ptr, unsigned long flag, unsigned long data);
int getWorkNum(work_queue **p);
#endif

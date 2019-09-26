#pragma once

struct workStruct
{
	unsigned long ptr;
	unsigned long flag;
	unsigned long data;
};

class workQueue
{
private:
	unsigned long maxSize = 128;
	unsigned long workNum = 0;
	pthread_mutex_t workMutex;
	unsigned long produce;
	unsigned long consume;
	struct workStruct * taskPoll = nullptr;
public:
	workQueue(long size);
	workQueue();
	~workQueue();
	bool getWork(void** ptr, unsigned long *flag, unsigned long *data);
	bool putWork(void* ptr, unsigned long flag, unsigned long data);
	int getWorkNum();
};
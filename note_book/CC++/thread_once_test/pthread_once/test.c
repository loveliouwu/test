#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<time.h>
#include<unistd.h>

pthread_once_t once = PTHREAD_ONCE_INIT;
void thread_once_work(void)
{
        int id = pthread_self();
        printf("thread_once_word start %d\n",id);
        return ;
}

void *task1(void *arg)
{
        int tid = pthread_self();
        printf("thread 1 id = %d\n",tid);
        pthread_once(&once,thread_once_work);
        printf("thread 1 start once_thread\n");
        sleep(10);
        return NULL;
}

void *task2(void *arg)
{
        int tid = pthread_self();
        printf("thread 2 id = %d\n",tid);
        pthread_once(&once,thread_once_work);
        printf("thread 2 start once_thread\n");
        return NULL;
}


void main()
{
        pthread_t thread1,thread2;
        pthread_create(&thread1,NULL,(void *)task1,NULL);
        sleep(2);
        pthread_create(&thread2,NULL,(void *)task2,NULL);

        sleep(5);

        return ;
}
### 基于管道（PIPE）的进程间通信   
管道并非属于进程的资源，而是和套接字一样，属于操作系统，所以，是两个进程通过操作系统提供的内存空间进行的通信。   
```c
#include<unistd.h> 
int pipe(int filedes[2]);
/*
成功时返回 0 ，失败时返回 -1
filedes[0]: 通过管道接收数据时使用的文件描述符，即管道出口
filedes[1]: 通过管道传输数据时使用的文件描述符，即管道入口
*/
```   
- demo  
```c
#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    int fds[2];
    char str[] = "Who are you?";
    char buf[BUF_SIZE];
    pid_t pid;
    // 调用  pipe 函数创建管道，fds 数组中保存用于 I/O 的文件描述符
    pipe(fds);
    pid = fork(); //子进程将同时拥有创建管道获取的2个文件描述符，复制的并非管道，而是文件描述符
    if (pid == 0)
    {
        write(fds[1], str, sizeof(str));
    }
    else
    {
        read(fds[0], buf, BUF_SIZE);
        puts(buf);
    }
    return 0;
}
```  

#### 双向管道   
```c
#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    int fds1[2], fds2[2];
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message";
    char buf[BUF_SIZE];
    pid_t pid;

    pipe(fds1), pipe(fds2);
    pid = fork();
    if (pid == 0)
    {
        write(fds1[1], str1, sizeof(str1));
        read(fds2[0], buf, BUF_SIZE);
        printf("Child proc output: %s \n", buf);
    }
    else
    {
        read(fds1[0], buf, BUF_SIZE);
        printf("Parent proc output: %s \n", buf);
        write(fds2[1], str2, sizeof(str2));
    }
    return 0;
}
```


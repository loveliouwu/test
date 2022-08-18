### clock  用于获取程序执行使用的时间  
- 头文件 <unistd.h>
- 注意，如果程序中有sleep，等导致线程睡眠的，睡眠时间不计入clock中
```c
#include<unistd.h>
void test()
{
    clock_t start = clock();
    int times = 100000;
    while(times--);
    clock_t end = clock();
    double cpu_time = ((double)(end-start))/CLOCKS_PER_SEC;
    printf("for loop took %f seconds to execute\n",)
}

```
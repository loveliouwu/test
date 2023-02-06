### assert()  
断言，是宏，而非函数，assert()宏的原型定义在<assert.h> <cassert>，其作用是如果它的条件返回错误，则终止程序执行，可以通过定义NDEBUG来关闭assert，但是需要在源代码的开头，在include<assert.h>之前

```c
#define NDEBUG //加上这一行，则assert不可用   
#include<assert.h>
assert( p != NULL); //则assert不可用
```

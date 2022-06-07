#include"test.h"

static const TEST test1 = {1,"str",NULL};

const TEST *return_test()
{
        return &test1;
}

// static const TEST *return_test()//如果添加static修饰，则该函数对外不可见，其他文件不能调用
// {
//         return &test1;
// }
#include<stdlib.h>
#include<stdio.h>

struct test_st
{
        int id;
        char *str;
        void *(func);
};

typedef struct test_st TEST;


const TEST *return_test();


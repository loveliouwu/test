#ifndef _DOUBLE_LIST_H_
#define _DOUBLE_LIST_H_

#include<stdlib.h>
#include<stdio.h>

#define LIST_LEN 64


typedef struct data_list
{
    int fd;
    char *fd_data;
}data_list;

typedef struct double_list
{
    data_list data;
    struct double_list *prev;
    struct double_list *next;
}double_list;

typedef struct list_info
{
    double_list *list_head;
    double_list *list_tail;
}list_info;

int Create_double_list(list_info **plist_info);

int En_double_list(list_info *list,void *data);

int isEmpyt_double_list(double_list *list);

int De_double_list(list_info *list,double_list *node);

int isFull_double_list(double_list *list);

int find_node(list_info *list,int fd,double_list **pnode);


#endif
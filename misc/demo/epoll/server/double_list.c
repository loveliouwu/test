#include"double_list.h"


#define LIST_ERR(fmt,arg...)    printf("[ERR %s:%s:%d]:"fmt,__FILE__,__func__,__LINE__, ##arg)
#define DEBUG
#ifdef DEBUG
#define LIST_DEBUG(fmt,arg...)    printf("[DEBUG %s:%s:%d]:"fmt,__FILE__,__func__,__LINE__, ##arg)
#else
#define LIST_DEBUG(fmt,arg...)
#endif


int Create_double_list(list_info **plist_info)
{
    double_list *head;
    list_info *info;
    info = (list_info *)malloc(sizeof(list_info));
    if(info == NULL)
    {
        LIST_ERR("malloc err!\n");
        return -1;
    }
    info->list_head = NULL;
    info->list_tail = NULL;
    *plist_info = info;
    return 0;
}


int isEmpyt_double_list(double_list *list)
{

}

int En_double_list(list_info *list,void *pdata)
{
    double_list *node;
    data_list *data = (data_list*)pdata;
    if(list == NULL)
    {
        LIST_ERR("list is null!\n");
        return -1;
    }
    node = (double_list *)malloc(sizeof(double_list));
    if(node == NULL)
    {
        LIST_ERR("malloc error!\n");
        return -1;
    }
    node->next = NULL;
    node->data.fd = data->fd;
    node->data.fd_data = data->fd_data;
    if(list->list_head == NULL)//如果链表为空
    {
        list->list_head = node;
        list->list_tail = node;
        node->prev = NULL;
    }
    else
    {
        list->list_tail->next = node;
        node->prev = list->list_tail;
        list->list_tail = node;
    }
    return 0;
}

int De_double_list(list_info *list,double_list *node)
{
    double_list *tmp;
    if(list == NULL)
    {
        LIST_ERR("list is null!\n");
        return -1;
    }
    if(node == NULL)
    {
        LIST_ERR("malloc error!\n");
        return -1;
    }
    if(node == list->list_head || node == list->list_tail)
    {
        if(node == list->list_tail && node == list->list_head)
        {
            list->list_head->next = NULL;
            list->list_tail->prev = NULL;
            list->list_head = NULL;
            list->list_tail = NULL;
        }
        else if(node == list->list_head)
        {
            list->list_head = list->list_head->next;
            list->list_head->prev = NULL;
        }
        else
        {
            list->list_tail = list->list_tail->prev;
            list->list_tail->next = NULL;
        }      
    }
    else
    {
        node->next = node->next->next;
        node->prev = node->prev->prev;
    }
    return 0;
    
}

int isFull_double_list(double_list *list)
{

}

int find_node(list_info *list,int fd,double_list **pnode)
{
    if(list == NULL)
    {
        LIST_ERR("list is null!\n");
        return -1;
    }
    if(fd <= 0)
    {
        return -1;
    }
    double_list *node;
    node = list->list_head;
    while(node!=NULL)
    {
        if(node->data.fd != fd)
        {
            node = node->next;
            continue;
        }
        else
        {
            *pnode = node;
            return 0;
        }
    }
    return 1;
}

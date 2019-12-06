#include <stdio.h>
#include <string.h>
#include "ini_parse.h"

/*
解析方式分为两种：
1.通过回调函数解析，用户修改回调函数模板
static int handler(void* user, const char* section, const char* name,
                   const char* value)
具体讲解看注释

2.通过配置文件显示ini文件内容
通过修改config.def文件添加CFG(section, key_name, default_key_value)
来从ini文件中读取键值
*/


#define HANDLER_MODE
#define CONFIG_MODE


//HANDLER_MODE
/*
user表示指向用户数据结果的指针，从ini_parse()函数中第三个参数传进来
session指向session的指针
name指向键
value指向键值
MATCH("a","b") a表示session，b表示键
*/
static int handler_mode(void* user, const char* section, const char* name,
                   const char* value)
{
    configuration* pconfig = (configuration*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("protocol", "version")) {
        pconfig->version = atoi(value);
    } else if (MATCH("user", "name")) {
        pconfig->name = strdup(value);
    } else if (MATCH("user", "email")) {
        pconfig->email = strdup(value);
    } else if (MATCH("zyxx", "email")) {
        pconfig->zyxx_email = strdup(value);
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}


//CONFIG_MODE
config_mode user_config = {
    #define CFG(s, n, default) default,
    #include "config.def"
};



int main()
{
#ifdef HANDLER_MODE
    configuration config;
    //第二个参数表示回调，第三个为存放结果的用户指针
    if (ini_parse("test.ini", handler_mode, &config) < 0) {
        printf("Can't load 'test.ini'\n");
        return 1;
    }
    printf("Config loaded from 'test.ini': version=%d, name=%s, email=%s, zyxx_email=%s\n",
        config.version, config.name, config.email, config.zyxx_email);
 


#endif

#ifdef CONFIG_MODE
    /* create one and fill in its default values */


    if (ini_parse("test.ini", config_handler_mode, &user_config) < 0)
        printf("Can't load 'test.ini', using defaults\n");
    dump_config(&user_config);
#endif

    return 0;

}
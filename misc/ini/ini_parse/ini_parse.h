


#ifndef __INI_PARSE_H_
#define __INI_PARSE_H_

#include"ini.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


/**********************************HANDLER_MODE*******************************************/
typedef struct  //自定义键值结构体，存放返回的键值
{
    int version;
    const char* name;
    const char* email;
    const char* zyxx_email;
} configuration;
static int handler_mode(void* user, const char* section, const char* name,
                   const char* value);




/**********************************CONFIG_MODE*******************************************/


/* define the config struct type */
typedef struct {
    #define CFG(s, n, default) char *s##_##n;
    #include "config.def"
}config_mode;

int config_handler_mode(void *user, const char *section, const char *name,
            const char *value);
void dump_config(config_mode *cfg);



#endif
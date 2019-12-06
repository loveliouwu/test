/* Simple test program
 *
 *  gcc -o test test.c minIni.c
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "minIni.h"

#define sizearray(a)  (sizeof(a) / sizeof((a)[0]))
const char inifile[] = "/home/lyp/projects/ini_min/test.ini";

int Callback(const char *section, const char *key, const char *value, void *userdata)
{
  (void)userdata; /* this parameter is not used in this example */
  printf("    [%s]\t%s=%s\n", section, key, value);
  return 1;
}

int main(void)
{
    char str[100];
    long n;
    int s, k;
    char section[50];

    /* string reading */
    /*section,key,default,save_buffer,buffer_size,ini_filename*/
    n = ini_gets("first", "string", "dummy", str, sizearray(str), inifile);//通过section和key值获取string型的数据，返回实际copy的字节数


    /* ----- */
    n = ini_gets("", "string", "dummy", str, sizearray(str), inifile);//如果section为空，遍历到第一个就返回
    n = ini_gets(NULL, "string", "dummy", str, sizearray(str), inifile);
    /* ----- */

    /* value reading */
    /*section,key,defaule,ini_filename*/
    n = ini_getl("first", "val", -1, inifile);//读取一个long型数据，返回long型键值
    n = ini_getl(NULL, "val", -1, inifile);
    /* ----- */


    /* string writing */
    n = ini_puts("first", "alt", "flagged as \"correct\"", inifile);
    n = ini_gets("first", "alt", "dummy", str, sizearray(str), inifile);
    /* ----- */
    n = ini_puts("second", "alt", "123456789", inifile);
    n = ini_gets("second", "alt", "dummy", str, sizearray(str), inifile);
    /* ----- */
    n = ini_puts(NULL, "alt", "correct", inifile);
    n = ini_gets(NULL, "alt", "dummy", str, sizearray(str), inifile);
    /* ----- */


    //list all section,key,value
    for (s = 0; ini_getsection(s, section, sizearray(section), inifile) > 0; s++) {
        printf("    [%s]\n", section);
        for (k = 0; ini_getkey(section, k, str, sizearray(str), inifile) > 0; k++) {
            printf("\t%s\n", str);
        } /* for */
    } /* for */

    /* browsing through the file */
    printf("browse through all settings, file field list follows\n");
    ini_browse(Callback, NULL, inifile);//通过回调函数获取所有数据

    /* string deletion */
    n = ini_puts("first", "alt", NULL, inifile);//删除一个key和value
    n = ini_puts("third", NULL, NULL, inifile);//删除一个section
    /* ----- */
    n = ini_puts(NULL, "alt", NULL, inifile);//无效

    return 0;
}


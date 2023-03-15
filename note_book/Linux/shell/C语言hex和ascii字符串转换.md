## ASCII字符串转HEX字符串   

["123456789"]  >>  ["313234353536373839"]

```c

unsigned int i = 0;
char hex[128];
char ascii[128] = "asciitohexstring";
unsigned int len = strlen((const char *)ascii);
for (i = 0; i < len; i++) {
		sprintf(hex + 2 * i, "%02X", ascii[i] & 0xff);
}
qDebug()<<"ASCII_string:"<<QString(ascii);
qDebug()<<"HEX_string:"<<QString(hex);

```   


## HEX字符串转为ASCII字符串  

["313233343536373839"]  >>  ["123456789"]

```c

char hex_string[]="313233343536373839";
char asciii[128];
unsigned int i = 0;
qDebug()<<strlen(hex_string);
for (i = 0; i < strlen(hex_string); i += 2) {
		sscanf(hex_string + i, "%2hhx", &asciii[i / 2]);
}
asciii[i/2]='\0';
qDebug()<<"ascii_string:"<<QString(asciii);

```

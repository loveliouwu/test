# od 
Octal Dump 以八进制或者其他格式转储文件  
od用于将指定文件内容以八进制、十进制、十六进制、浮点格式或者ASCII编码字符方式显示，通常用于显示或者查看文件中不能直接显示在终端的字符，默认显示方式是八进制   

### -A 后续跟[doxn]中的一个 选择以何种基数表示地址偏移   
```shell
od -A x test.out   //以16进制表示
od -A o test.out	//8进制
od -A d test.out	//10进制  
od -A n	test.out	//不带地址
```  


### -t 指定输出格式，格式包括a、c、d、f、o、u和x   
- a 具名字符，比如换行符显示为nl
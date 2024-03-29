#!/bin/sh  

- ";"一行多个命令分隔符  

- "&"后台执行命令   
如果两个命令中间用"&"隔开，shell将在后台执行其前面的命令，而不用等到该命令完成就可以执行下一个命令  
`ls & ls`   

- 变量   
变量名称的开头是一个字母或下划线符号后面可以接着任意长度的字母、数字、下划线，Shell变量可以用来保存字符串值，所能保存的字符数没有限制。
**注：=号两边必须都没有空格**
```shell
my_val="lyp"
my_val=lyp   ##等效上一条

my_val1=$my_val  #将一个变量的值赋给另一个

my_val2=$my_val$my_val1  #两个变量合为一个
```


- \ 转义字符  
\b : 退格  
\c : 输出种忽略最后的换行字符，这个参数后的任何字符都会被忽略掉
\t : 水平制表符  
\v : 垂直制表符
1、使用转义字符打印信息时需要添加参数'-e'  
2、包含转义字符的字符串需要用单引号或双引号
```shell
echo "\n\n"  #打印结果 nn
echo -e "\n\n" #打印两个空行  1、-e 2、带引号
#\b 退格 
echo -e "abcd\b1234" #abc1234 
#\c
echo -e "abcd\c1234"  #结果为abcd   
```

- -x sh 参数，开启跟踪模式   
如果我们调试命令时，需要看到当前执行的命令信息，则添加参数-x  
sh -x test.sh  
同样的，可以在脚本中在需要的位置设置  
方法1： **sh -x test.sh**
方法2：在shell脚本中添加`set -x`开启跟踪，`set +x`关闭跟踪
```shell
echo "test -x"
set -x #需要的地方开启跟踪  
my_val=sdf
set +x #不需要的地方关闭跟踪
echo $sdf
```  

- grep  
-E：使用正则表达式进行匹配，可取代egrep  
-F：使用固定字符串进行匹配，可取代fgrep
-e：指定后面的参数为模式，即使以减号开头
    ```shell
    set -x 
    >>grep -e -x   
    >>set -x
    ```
-i：忽略大小写差异  
-l：列出匹配模式的文件名称，而不是打印匹配行
-f：从文件读取模式作匹配
```shell
grep -ie ^abcd
grep -ie "^[abcde]\{2,\}"
```

- 使用${val_name:-val}、${val_name:=val}来对变量进行扩展  
```shell
read a  
b=${a:-100}  #如果a有值，则b=a,如果a为空则b=100  
c=${a:=100}  #如果a有值，则c=a,如果a为空则c=100,同时a=100
```   


### cat <<EOF  
这里的EOF没有特殊的意义，只是将EOF作为输入结束的标识，可以是其他字符： cat <<end  
当输入的最后一行为end时判断为输入完成。   

### cat >file_name  
将标准输入输入到文件中   

### cat >file_name <<END  
将标准输入输出到文件中，并且判断END时结束
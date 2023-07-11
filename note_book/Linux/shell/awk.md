## awk

### awk中的特殊变量   
- FN 记录列的数量   
- NR 记录当前记录的数量即行数   
- %N %0表示整行的内容，$1表示第一列数  


### -F 选项是用来重新定义默认字段分隔字符，一贯用作第一个命令参数  
```shell


SH_NAME=${0##*/}  #获取sh脚本的文件名
SH_PATH=$( cd "$( dirname "$0" )" && pwd )  #获取脚本的路径  

awk -F '\t' 
awk -F ';'
awk -F '::'
```   

### awk 格式化解析文件
awk 'printf "%-8s %-8s\n" , $1， $2'  sample.txt   


### awk 设置变量  
awk -v a=1 'printf $1, $1+a' smaple.txt (1)
>>>1 2


### 过滤第一列不是2的行  
awk '$1!=2' smaple.txt   
awk '$1!=2&&$3!="this"'  sample.txt

### 过滤包含re字符的行  
awk '!/re/'  sample.txt
awk '$3~/re/' sample.txt  #过滤某一列包含re的行


### 输出文件名，当前行，列数，行数  
awk 'print FILENAME, $0, NF, NR'  sample.txt



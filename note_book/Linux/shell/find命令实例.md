### find  
find用于根据指定的参数搜索和定位文件和目录的列表。find命令可以在多种情况下使用，比如你可以通过权限、用户、用户组、文件类型、日期、大小和其他可能的条件来查找文件。 

- 使用find在指定目录下查找文件  
`find /etc -name inittab`  
`find /etc -iname example` -i不区分大小写 
`find . -type d -name tmp`  指定文件类型为目录 
```shell
find . -type d -name test  #指定类型为目录，名字为test 
find . -type f -iname "*.sh" #指定类型为文件，不区分大小写，匹配后缀
find . -type f -name "*.c" -name "sotp_*" #同时用多个参数 
find . -type f -perm 0777  #文件权限是777的所有文件  
find . -type f ! -perm 777 #文件权限不是777的所有文件
find . -mtime +30 #30天以前修改的文件
find . -mtime -3 #3天以内修改的文件
find . -mtime +30 -mtime -60 #30天以前60天以内修改的文件
find . -cmin -60 #60分钟以内修改过的文件
find . -size +50MB -size -100MB #大于50小区100的文件

```

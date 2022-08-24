### 回调命令历史  
bash将命令历史保存在缓冲区或是默认文件`~/.bash_history`，命令历史缓冲区可以保存很多命令，其数量由HISTSIZE定义  
 
`Ctrl + r`键可以搜索历史命令  

- `!!`  输入双叹号可以执行上一条命令   

- `!git`  输入!加上制定字符开头的命令，可以回调最近一次执行的该字符开头的命令。  

- `!103`  通过命令history获取命令历史后，可以根据行号来执行相应的命令  




### shell中的扩展   
```shell
echo a{b,d,e}c
>>abc adc aec

echo a{a..c}c
>>aac abc acc

echo {0..4}
>>0 1 2 3 4

echo {0..3}{a..c}
>>0a 0b 0c 1a 1b 1c ...
```  

#### 使用场景  

- `touch ./test{0..10}.txt`用来批量创建文件   




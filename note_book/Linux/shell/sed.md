## sed 用来处理文本文件  
`sed [-hnV][-e<script>][-f<script文件>][文本文件]`


### 动作说明   
- a ：新增， a 的后面可以接字串，而这些字串会在新的一行出现(目前的下一行)～
- c ：取代， c 的后面可以接字串，这些字串可以取代 n1,n2 之间的行！
- d ：删除，因为是删除啊，所以 d 后面通常不接任何东东；
- i ：插入， i 的后面可以接字串，而这些字串会在新的一行出现(目前的上一行)；
- p ：打印，亦即将某个选择的数据印出。通常 p 会与参数 sed -n 一起运行～
- s ：取代，可以直接进行取代的工作！通常这个 s 的动作可以搭配正则表达式！例如 1,20s/old/new/g

### 例子  
```shell
sed -e 4a\newline testfile.txt  #在testfile文件的第四行后添加一行数据“newline” 并将结果输出到标准输出  

sed -e '2,5d' testfile.txt  #从第2行开始，删除5行内容  
#注： 可以通过 `nl testfile.txt` 或 `cat -b testfile.txt` 查看行号   

sed -e '3,d' testfile.txt #删除从第三行开始到最后的内容  






```
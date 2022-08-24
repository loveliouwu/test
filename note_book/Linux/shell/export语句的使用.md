### export介绍   
运行shell脚本时，系统将创建一个shell，当此shell脚本运行完毕时，这个子shell将终止，环境将返回到执行该脚本运行之前的shell。默认情况下，所有用户定义的变量只在当前shell内有效，它们不能被后续的shell引用，要使某个变量可以被子shell引用，可以使用export命令将变量进行输出。   
Bash的内置命令export会将指定给它的变量或函数自动输出到后续命令的执行环境。export命令的语法如下所示：
`export [-fnp] [变量或函数名称]=[变量设置值]`    
-f 选项表示export一个函数
-n 选项表示将export属性从指定变量或函数上移除，-p选项打印当前shell所有输出的变量，与单独执行export命令结果相同。    
$JAVA_HOME=/usr/local/jdk 

```shell
read TEST_SHELL
>>123123123
echo $TEST_SHELL
export TEST_SHELL
bash  #new shell
echo $TEST_SHELL:
```

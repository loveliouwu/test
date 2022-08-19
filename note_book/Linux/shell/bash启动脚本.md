### Bash启动脚本
> 在用户登陆时自动执行的脚本主要用于设置一些环境，例如设置JAVA_HOME的路径，加载驱动模块等，其中的一些脚本被登陆shell调用，登陆shell是你登陆系统时最先执行的shell，它设置一些环境，然后把这些环境授权于非登陆shell。  
### 脚本及说明  
- `/etc/profile`	当用户在运行级别3登陆系统时首先运行   
- `/etc/profile.d`  当/etc/profile运行时，会调用该目录下的一些脚本 
- `$HOME/.bash_profile`、`$HOME/.bash_login`、`$HOME/.profile`  在/etc/profile运行后，按顺序第一个存在的被执行。
- `$HOME/.bashrc`  在上述脚本中的一个运行后即调用此脚本  
- `/etc/bashrc`或`/etc/bash.bashrc`  由`$HOME/.bashrc调用运行` 


### Bash启动脚本主要设置的环境有：
1、 设置环境变量PATH和PS1；
2、 通过变量EDITOR设置默认的文本编辑器；
3、 设置默认的umask(文件或目录的权限属性)；
4、 覆盖或移除不想要的变量或别名；
5、 设置别名；
6、 加载函数。

### 当退出时，会执行bash退出脚本   
1、 使用clear命令清理终端屏幕输出；
2、 移除临时文件
3、 自动运行一些命令或脚本。

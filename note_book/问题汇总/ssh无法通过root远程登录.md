# 解决无法通过root远程ssh链接的问题  

### 首先排除是否存在root密码  
如果不存在密码则`sudo passwd root`设置密码  


### 如果存在，则修改以下配置文件  
step 1： `/etc/ssh/sshd_config`  
step 2: 找到`PermisRootLogin`行  改为 `PermisRootLogin yes`  
step 3：sudo /etc/init.d/ssh restart  


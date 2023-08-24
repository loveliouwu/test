### 问题分析  
Linux里面有两套管理网络连接的方案：  
1、/etc/network/interfaces  (/etc/init.d/networking)
2、Network-Manager  

两套方案是冲突的，不能同时共存  
第一个方案用于服务器等一些不需要经常改动连接的场合，第二套适用于有桌面的环境，网络连接经常会变的。  


### 解决方案  
为了避免冲突，又能共享配置，故：  
1、当Network-Manager发现/etc/network/interfaces被改动的时候，则关闭自己（**即 显示未托管**），除非managed (**/etc/NetworkManager/NetworkManager.conf**) 设置为**true**    
2、当managed被设置为真时，/etc/network/interfaces，则不生效    


### 具体操作  
sudo vim /etc/NetworkManager/NetworkManager.conf  
修改 managed=false 为 managed=true
sudo service network-manager restart
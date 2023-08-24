## 微调次数  
例如ubuntu内核版本为： 5.4.0-139： 分别代表 主版本号：次版本号：修正号：微调次数

如果要降低版本：
sudo apt-get install linux-image-5.4.0-139-generic 
sudo apt-get install linux-headers-5.4.0-139-generic
sudo vim /etc/default/grub 

修改
GRUB_DEFAULT=0
为
GRUB_DEFAULT="Advanced options for Ubuntu>Ubuntu, with Linux 5.4.0-139-generic"

sudo update-grub

sudo reboot 
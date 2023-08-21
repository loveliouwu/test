## kvm-qemu 


### kvm-qemu 安装  

#### 依赖安装包及其功能
qemu-kvm -为KVM管理程序提供硬件仿真的软件。
libvirt-daemon-system -用于将libvirt守护程序作为系统服务运行的配置文件。
libvirt-clients -用于管理虚拟化平台的软件。
bridge-utils -一组用于配置以太网桥的命令行工具。
virtinst -一组用于创建虚拟机的命令行工具。
virt-manager -易于使用的GUI界面和支持命令行工具，用于通过libvirt管理虚拟机。

### apt安装KVM/qemu/virt-manager
sudo apt install kvm-qemu
sudo apt install virt-manager

### 源码安装qemu/libvirt/virt-manager

-------------------------------------------------------------------------------------------
编译安装QEMU
-------------------------------------------------------------------------------------------
sudo apt-get install ninja-build gcc g++ libglib2.0-dev  meson libpixman-1-dev libspice-server1  libspice-protocol-dev libspice-server-dev cmake qemu-utils qemu-system-common

tar -xvf ./qemu-7.2.4.tar.xz

./configure --prefix=/usr --localstatedir=/var  --sysconfdir=/etc --enable-kvm --target-list=x86_64-softmmu   --enable-vhost-kernel --enable-vhost-net --enable-vhost-user --enable-vhost-crypto  

make -j4

sudo make install
-------------------------------------------------------------------------------------------
编译安装libvirt
-------------------------------------------------------------------------------------------
sudo apt-get install libxml2-utils xsltproc python3-pip  python-docutils libgnutls30  libgnutls28-dev gnutls-bin libxml2 libxml2-dev libyajl-dev libtirpc-dev  libghc-gnutls-dev libglib2.0-dev libudev-dev ninja-build meson nftables libnl-3-dev libnl-route-3-dev libpciaccess-dev

tar -xvf ./libvirt-8.0.0.tar.xz

 --reconfigure

meson setup build  --prefix=/usr/  -Ddriver_libvirtd=enabled -Dsystem=true -Ddriver_remote=enabled -Ddriver_network=enabled -Ddriver_qemu=enabled -Ddriver_interface=enabled -Dlibnl=enabled

meson configure

ninja -C build


sudo ninja -C build install



-------------------------------------------------------------------------------------------
安装virt-manager 
-------------------------------------------------------------------------------------------
sudo apt-get install python3-pip python3-libxml2 python3-requests gettext python3-docutils libvirt-glib-1.0-dev libosinfo-1.0-dev python3-libvirt libgirepository1.0-dev libcairo2-dev

pip3 install setuptools
安装pycairo
python3 ./setup.py install 
安装PyGObject
python3 ./setup.py install

tar -xvf ./virt-manager-4.1.0.tar.gz
sudo python3 ./setup.py install












### qemu 使用  
- 创建一个虚拟机空间
qemu-img create -f qcow2 OS1.img 10G 


- 创建虚拟机  
qemu-system-x86_64 -boot d -cdrom ubuntu-20.04-desktop-amd64.iso -hda OS1.img -m 2048

    qemu-system-x86_64命令是结合自己的电脑构架使用的，因为这边使用的是Intel x64平台；
    boot d：表示从CDROM启动系统，因为虚拟盘中目前还没有系统，需要从CDROM中启动安装盘；
    cdrom：指定了安装的系统镜像；
    hda：指定之前创建的虚拟盘；
    m：指定了QEMU使用的系统内存大小，这里指定的是2G；

	- 快捷键  ctrl+alg+G 切换鼠标窗口




## XML 配置描述  

### os  
- type   
指定了要在虚拟机中启动的操作系统的类型，hvm表示该操作系统是为了在裸机上运行而设计的，rach制定要虚拟化的CPU架构，machine指的是机器类型
```xml
<type arch='x86_64' machine='pc' >hvm</type>  

```





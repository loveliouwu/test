第一部分是删去引导，第二部分是删去系统。  



删去引导首要进入windows系统下，搜索cmd翻开命令行，顺次输入：
```shell
diskpart # 进入系统的diskpart组件
list disk # 检查系统所有的磁盘，并找到windows系统所装置的磁盘
select disk  x # x是windows所装置的磁盘，由上一步得到，自己检查
list partition # 找到类型为系统的分区
select partition x # x为类型为系统的分区,由由上一步得到，自己检查
assign letter=p # 将分区分配到磁盘P中，分配到哪个磁盘其实随意，只需不和现有的磁盘冲突即可
exit  # 退出
```
退出后翻开会发现多出一个P盘来，也就是说你还能够设置X，Y，Z盘 
点击P盘你会发现权限不够， 通过WINDOWS SHELL 管理员权限可以访问这个磁盘，会发现有个EFI文件夹，翻开EFI文件夹里面就是引导文件夹，直接删去Ubuntu的文件夹，然后从头运用
```shell  
diskpart：diskpart # 再次进入到disk
partselect  disk x # x是之前挑选的盘符
select partition x # x是之前挑选的分区
remove letter=p # 删去P盘，执行完上面的操作后，引导也就删去成功了。
```
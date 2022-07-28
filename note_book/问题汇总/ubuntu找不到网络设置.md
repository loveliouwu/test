```shell
sudo vim /etc/NetworkManager/NetworkManager.conf
#修改managed = true
sudo service network-manager restart
```


- 若还不行，则进行下面步骤  
```shell
sudo service network-manager stop
sudo gedit /var/lib/NetworkManager/NetworkManager.state 
#修改 NetworkingEnabled = true
sudo service network-manager start

```
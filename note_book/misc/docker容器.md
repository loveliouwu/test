# Docker    
docker是一个开源的应用容器引擎，基于Go语言，可以让开发者打包他们的应用以及依赖包到一个轻量级、可移植的容器中，然后发布到任何流行的Linux机器上，也可以实现虚拟化。  
容器是完全使用沙箱机制，相互之间不会有任何接口，更重要的是容器性能开销极低。  

### 应用场景  
- Web应用的自动化打包和发布  
- 自动化测试和持续集成、发布  
- 在服务型环境中部署和调整数据库或其他的后台应用  

### Docker架构  
- **镜像**：镜像Image，就相当于是一个root文件系统  
- **容器**：镜像和容器的关系，就像是面对对象程序设计中的类和实例一样，镜像是静态的定义，容器是镜像运行的实体，容器可以被创建、启动、停止、删除、暂停等。  
- **仓库**：仓库可以看成一个代码控制中心，用来保存镜像。   



### 常用命令  
- 创建镜像  
    - 下载镜像： sudo docker pull ubuntu:20.04  
    
- 创建容器
    - 一般的命令：docker run ubunto:20.04
    - 指定网络：docker run -it --net=host dockername_ubuntu20 /bin/bash 
    - 设备共享：docker run --device=/dev/pe100_dev ubuntu20

- 其他指令  
    - 获取当前的容器列表  docker ps -a 
    - 删除容器  docker rm dockerid
    - 切换到容器bash   docker attach dockerid
    - 启动容器  docker start docketid
    - 获取镜像列表  docker image ls


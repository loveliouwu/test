# django 

- 安装django  
`sudo apt install python3-django`  

- 创建django工程  
`django-admin startproject hellodjango`  
切换到创建的工程路径下，执行   
`python3 manager.py runserver`  


- 修改配置文件   
配置文件路径**projectpath/setttings.py**  
	- 修改语言代码  
	```python 
		LANGUAGE_CODE = 'zh-hans'
		TIME_ZONE = 'Asia/Chongqing'
		
	```
	
	
- 创建应用 
`python3 manager.py startapp myapp`  
执行后会在当前路径下创建myapp目录  
	- __init__.py :一个空文件，告诉python解释器这个目录应该被视为一个python的包   
	- admin.py :
	可以用来注册模型，用于在Django的管理界面管理模型   
	- apps.py : 
	当前应用的配置文件    
	- migrations :    
	存放与模型有关的数据库迁移信息    
	- models.py : 
	存放应用的数据模型，即实体类及其之间的关系  
	- tests.py  :  
	包含测试应用各项功能的测试类和测试函数  
	- view.py  
	处理请求并返回响应的函数    
	
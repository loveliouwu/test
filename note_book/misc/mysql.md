### 基本概念  
- mysql和sql的区别   
mysql是一个DBMS(数据库管理系统),是市场上第一个可用的开源数据库之一，是数据库软件   
SQL用于访问、更新、操作数据库中的数据的一种语言，是结构化查询语言。

### SQL  
结构化查询语言(Structured Query Language)的缩写，SQL是一种专门用来与数据库通信的语言。  
优点：  
1、不是某个特定数据库供应商专有的语言，几乎所有重要的DBMS都支持SQL。  
2、简单易学   
3、灵活，可以进行非常复杂和高级的数据库操作。   
SQL用关键字、表名、列名等组合而成的一条语句(SQL语句)来描述操作的内容，   
根据对RDBMS赋予的指令种类的不同，SQL语句可以分为以下三类。  
- DDL （Data Definition Language,数据定义语言）用来创建或者删除存储数据用的数据库以及数据库中的表等对象：   
**CREATE**：创建数据库和表等对象  
**DROP**：删除数据库和表等对象  
**ALTER**：修改数据库和表等对象的结构   

- DML （Data Manipulation Language,数据操纵语言）用来查询或者变更表中的记录。  
**SELECT**：查询表中的数据    
**INSERT**：向表中插入新数据   
**UPDATE**：更新表中数据    
**DELETE**：删除表中数据  

- DCL （Data Control Language,数据控制语言）用来确认或者取消对数据库中数据进行的变更，除此之外，还可以对RDBMS的用户是否有权限操作数据库中的对象进行设定。  
**COMMIT**：确认对数据库中的数据进行变更  
**ROLLBACK**：取消对数据库中的数据进行的变更  
**GRANT**：赋予用户操作权限   
**REVOKE**：取消用户的操作权限   





- 表  
某种特定类型数据的结构化清单。  数据库中每一个表都有一个名字，用来标识自己，此名字是唯一的。

- 列  
表由列组成，列中存储着表中某部分的信息。  

- 行  
表中每一行都应该有可以唯一标识自己的一个列。  

- 主键   
主键列，其值能够唯一区分表中每一行。主键通常定义在表的一列上，但这并不是必需的，也可以一起使用多个列作为主键，在使用多列作为主键时，上述条件必须应用到构成主键的所有列，所有列的值的组合必须是唯一的，但单个列的值可以不唯一  

### 数据类型   
数据类型表示数据的种类，包括数字型，字符型，日期型等，每一列都不能存储与该列数据类型不符的数据。   
- INTEGER   
    用来存储整数的数据类型   
- CHAR   
    字符串类型，可以像`CHAR(10)`或者`CHAR(1000)`这样，在括号中指定该列可以存储的字符串的长度，字符串超出最大长度的部分是无法输入到该列中的。  
- VARCHAR   
    同CHAR类型一样，存储字符串，也可以通过括号来指定最大长度，但该类型的列是以可变长字符串的形式来保存字符串的  

- DATE  
    用来指定存储日期的列的数据类型   
     
- 约束的设置    
    约束是除了数据类型之外，对列中存储的数据进行限制或者追加条件的功能。   
```SQL
product_id CHAR(4) NOT NULL,//该列设置了不能输入空白  
PRIMARY KEY (product_id), //设置主键约束
```


### 基本操作  

- 安装和登陆  
```shell
sudo apt-get install mysql-server

//mysql可以接受命令行参数，如为了指定用户登陆名、主机名、端口、口令  
mysql -u ben -p -h myserver -P 9999
sudo mysql -uroot -p  

//进入命令行模式后  
\h获得帮助  
help select //获得使用SELECT语句的帮助
quit、exit退出命令行实用程序。 
``` 

- 创建新数据库  
`CREATE DATABASE mytest;`   

- 选择数据库  
`use mytest;`   
必须先使用use打开数据库，才能读取其中的数据。  

- 了解数据库和表  
`show databases;`  
该命令返回可用数据库的一个列表，包含在在这个列表中的可能是MYSQL内部使用的数据库。   
`show tables;`  
该命令返回数据库列表中表的列表。   
`show columns from customers;`  
显示列表的信息   
`describe customers;`    
作为show columns from 的一种快捷方式。 
`show status;`  
用于显示广泛的服务器状态信息。  
`show grants;`  
用于显示授予用户的安全权限。  

### 创建表和删除  
```sql  
create table <表名> {
    <列名> <数据类型> <该列所需约束>,
    <列名> <数据类型> <该列所需约束>,
    ...
    <该表的约束1>,<该表的约束1>,...
};

create table testtable(
    id INTEGER, 
    name CHAR(20) not null,
    PRIMARY KEY (id)
);

//删除  
DROP TABLE tablename;
//删除的表是无法恢复的，即使是被误删的表，也无法恢复，只能重新创建，然后重新插入数据。
```

### 表定义的更新  （ALTER TABLE语句）  
如果需要变更表的信息，可以使用ALTER TABLE语句  
`ALTER TABLE <表名> ADD COLUMN <列的定义>；`   
`ALTER TABLE <表名> DROP COLUMN <列名>;`
```sql
//添加列
ALTER TABLE product ADD COLUMN product_name VARCHAR(100);
//删除列
ALTER TABLE product DROP COLUMN product_name;

```

### 修改表名   
```SQL
//oracle  
alter table product rename to product_name;
//mysql
rename table product to product_name;
```

### SELECT  
`SELECT <列名>, ... FROM <表名>`  
该SELECT语句包含了SELECT和FROM两个子句，子句是SQL语句的组成要素，是以SELECT或者FROM等作为起始的短语。   

- 基本使用
```sql  
select column_name from table_naem;
select col_1,col_2,col_3 from table_1;
select * from table_1;
```  
- 为列设定别名   
使用`AS`关键字为列设定别名  
```sql
select product_id as id, product_name as name from product;  
//别名可以使用中文，使用中文时需要用双引号括起来。  
select product_id as "商品编号" from product;
```

- DISTINCT 去重   
告诉mysql只返回不同的值。可以多列之前使用，此时多个列的数据进行组合，将重复的数据合并为一条，即多个列同时重复则为重复
```sql  
select DISTINCT vend_id from products;  
```  

- 根据where语句来选择记录  
select语句通过where子句来指定查询数据的条件，可以指定某一列的值和这个字符串相等，或某一列的值大于这个数字等条件。   
```sql
select <列名> ... from <表名> where <条件表达式>； 
select product_name,product_type from product where product_type = "衣服"；
```

- LIMIT 限制结果  
select返回匹配的行，为了返回第一行或前几行，可使用limit子句：  
```sql
//只显示不多于5行 
select prod_name from products LIMIT 5; 
//从第5行开始的5行，包括第5行
select prod_name from products LIMIT 5,5;  

``` 

- 使用完全限定的表名  
```sql
select products.prod_name from products;
select products.prod_name from table_1.products;
```  


### 算数运算符和比较运算符  
主要支持的运算符有：+ - * /
```sql
select product_name, sale_price, sale_price * 2 AS "sale_price_x2" from product;

```




### 插入数据   

```SQL
insert into product values('001', 'T恤衫'， '衣服'， 500， '2021-09-26');  
```
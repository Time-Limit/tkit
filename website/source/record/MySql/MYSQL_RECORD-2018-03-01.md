# MySQL 学习笔记

## 2018-03-01

### 忘记密码的处理方法

> vim /etc/my.cnf

> 在 [mysqld] 下添加 skip-grant-tables

> serice mysqld restart

### 修改密码

> use mysql;

> update user set authentication_string=password("p") where user="u";

> flush privileges;

> 退出并重启MySQL

### 《SQL必知必会》 第一章

#### 一些定义

* 数据库: 保存有组织的数据的容器。
* 表: 某种特定类型数据的结构化清单。
* 模式: schema. 关于数据库和表的布局及特性的信息。
* 列: 表中的一个字段。所有表均有一个或多个列组成。
* 行: 表中的一条记录。
* 主键: 一个或一组列，其值能够唯一标识表中的每一行。
* SQL: struct query language. SQL 是一种专门用来和数据库沟通的语言。

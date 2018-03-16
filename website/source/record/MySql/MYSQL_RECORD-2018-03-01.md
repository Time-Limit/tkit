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




## 2018-03-02

### 《SQL必知必会》 第二章

#### SELECT

> 检索单列 : SELECT col_name FROM table_name;

> 检索多列 : SELECT col1_name, col2_name, col3_name FROM table_name;

> 使用通配符 : SELECT * FROM table_name;

> 剔除检索结果的相同记录 : SELECT DISTINCT cn1, cn2, cnd3 FROM table_name;

> 只显示检索结果的前n行 : SELECT * FROM table_name LIMIT n;

> 只显示检索结果中从 m 行起的第 n 行 : SELECT * FROM tn LIMIT n OFFSET m;

#### 添加注释

> SELECT * FROM tn;  -- 这是注释

> /\* 这是注释

>    这也是注释 \*/

### 《SQL必知必会》 第三章

```
    关系数据库设计理论认为，如果不明确规定排序顺序，则不应该假定检索的出的数据的顺序有任何意义。
```

#### 定义:
* 子句: SQL语句由必须或可选的子句构成。一个子句通常由关键字+数据组成。

#### ORDER BY 子句

> 按指定列排序 : SELECT cna, cnb, cnc FROM tn ORDER BY cnx, cny, cnz;

> 按降序排序 : SELECT * FROM tn ORDER BY cnx DESC, cny, cnz DESC;

```
书中说 ORDER BY 字句必须是 SELECT 的最后一个子句。
但是 SELECT * FROM table LIMIT n ORDER BY cn; 是错误的。
So，如何界定最后一个子句呢。
```



## 2018-03-04

###《SQL必知必会》 第四章

### 使用 WHERE 子句

```
SELECT * FROM table_name WHERE col_name = value; 
```

### WHERE 子句操作符

* = 等于
* <> 不等于
* != 不等于
* < 小于
* <= 小于等于
* !< 不小于
* \> 大于
* \>= 大于等于
* !\> 不大于
* BETWEEN 在指定的两个值之间 (SELECT * FROM tn WHERE cn BETWEEN min AND max;)
* IS NULL 为NULL值(确定值为 NULL，不能简单的使用 = NULL。 而应该使用 IS NULL。)


### 《SQL必知必会》 第五章

为了进行更强的过滤控制，SQL 允许出现多个 WHERE， 用 AND 或者 OR 连接。
当 WHERE 字句中出现OR 和 AND 的组合时，应该用 () 指定求值顺序。
```
SELECT * FROM tn WHERE (cn = v OR cn = x) AND cn2 = z; 
```

### IN 操作符
IN 操作符用来指定条件范围，范围中的每个条件都可以匹配。效果等价于多个 OR。
```
SELECT * FROM tn WHERE cn IN (v1, v2, v3);
```

### NOT 运算符
用来否定紧随其后的条件。

```
SELECT * FROM tn WHERE NOT cn IN (v1, v2) OR cn = v3;
SELECT * FROM tn WHERE NOT (cn IN (v1, v2) OR cn = v3);
```



## 2018-03-05

### 《SQL必知必会》 第六章

### 定义
* 通配符:用来匹配值的一部分特殊字符。
* 搜索模式:由字面值，通配符或两者构成的搜索条件。

```
若想使用通配符，则应使用 LIKE 操作符。
通配符只能用来搜索文本字段。
```

### 百分号(%)通配符
在搜索模式中，'%' 表示任意字符出现任意次数(0次，1次，或多次)。
```
SELECT * FROM tn WHERE cn='%something%'
```
### 下划线(_)通配符
在搜索模式中，'_' 可以匹配单个任意字符。
```
SELECT * FROM tn WHERE cn='s_meth_ng'
```

### 《SQL必知必会》 第七章

### 使用 Concat 拼接字段

```
SELECT TRIM(Concat(RTRIM(cn_a), ' ', LTRIM(cn_b))) AS RESULT_TITLE FROM tn;
```

TRIM, LTRIM, RTRIM 去除对应位置的空格。
AS 为列定义一个别名。

SQL 也支持加减乘除操作。
```
SELECT (3+5)/2*666;
```



## 2018-03-06

### GROUP BY 和 HAVING

在 SELECT 语句中， 可以用 GROUP BY 对结果集进行分组，用 HAING 对结果集进行过滤。
```
SELECT CONCAT(cnx, '(', cny, ')') AS title, COUNT(cnz) AS cnt FROM tn GROUP BY title HAVING cnt > 4 ORDER BY cnt DESC;
```



## 2018-03-07

### 嵌套子查询

```
SELECT tn.cnx, tn.cny FORM tn WHERE tn.cny IN (SELECT tn1.cnz FROM tn1);

SELECT tn.cnx, tn.cny, (SELECT COUNT(*) FROM tn1) AS cnt FROM tn;
```

当子查询结果集用于父查询的WHERE时，应该只包含一列。
当子查询结果集合是父查询结果集的一部分时，应该只有一行一列。



## 2018-03-08

用 SELECT 在多个表中查询数据
```
SELECT * FROM tn1, tn2, tn3;
```

自连接
```
SELECT A.cn1, A.cn2, B.cn1 FROM tn AS A, tn AS B;
```

外连接
```
SELECT cn1, cn2 FORM tn1 LEFT OUTER JOIN tn2 WHERE tn1.cn3 == tn2.cn3;
```

* LEFT OUTER JOIN : 左向外连接，当左表某行在右表中没有匹配行时，该记录右表中的列为NULL。
* RIGHT OUTER JOIN : 右向外连接，当右表某行在左表中没有匹配行时，该记录左表中的列为NULL。
* FULL OUTER JOIN : 全外连接，显示所有行，当不匹配时，另一张表的列显示为NULL。



## 2018-03-09

### UNION

* 合并多条 SELECT 语句的查询结果。
* UNION 合并的每个查询必须具有相同的列，表达式或聚集函数。
* 列类型必须兼容。
* UNION 会过滤重复记录。UNION ALL 会保留所有记录。
* 只允许有一个 ORDER_BY 在 最后一个 SELECT 后面。这个 ORDER_BY 会排序所有的结果。



## 2018-03-13

### INSERT

* 插入完整的行
* 插入行的一部分
* 插入查询的结果

```
    INSERT INFO tn(c1, c2, c3) VALUES(v1, v2, v3);
```

```
INSERT INFO tn(c1, c2, c3) SELECT c1, c2, c3 FROM tn1;
```

### 当要插入省略列时需满足下述条件

* 省略列定义为允许NULL值。
* 省略列在表定义时给出了默认值。

### 创建表时复制

```
CREATE TABLE　new_tn AS SELECT * FROM tn;
```

复制指定列

```
CREATE TABLE new_tn AS SELECT cn, cn1 FROM tn;
```

### UPDATE

```
UPDATE tn SET cn1 = v1, cn2 = v2 WHERE cn3 IN (SELECT cn4 WHERE cn4 = v4);
```

UPDATE 的 子查询语句中不能用FROM指定表。



## 2018-03-14

### DELETE 删除表的内容

```
DELETE FROM tn;
```
```
DELETE FROM tn where cn = val;
```

### 创建表

	CREATE TABLE table_name
	(
	cn1 CHAR(10) NOT NULL,
	cn2 CHAR(10) NOT NULL DEFAULT 'test',
	);

### 修改表

	ALTER TABLE table_name
	ADD new_column CHAR(10);

	ALTER TABLE table_name
	DROP old_column;


### 删除表

    DROP TABLE table_name;



## 2018-03-15

### 视图

视图就像其他编程语言的函数一样，用以封装可重复利用的SQL语句。

	CREATE VIEW view_name AS
	SELECT cn1, cn2 FORM table_name;

	DROP VIEW view_name;

	show table status where comment = 'view' \G;

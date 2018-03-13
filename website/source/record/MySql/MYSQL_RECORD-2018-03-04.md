# MySQL 学习笔记

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


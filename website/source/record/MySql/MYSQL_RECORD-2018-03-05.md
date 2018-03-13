# MySQL 学习笔记

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

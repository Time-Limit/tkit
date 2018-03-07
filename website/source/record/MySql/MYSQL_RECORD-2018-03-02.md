# MySQL 学习笔记
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

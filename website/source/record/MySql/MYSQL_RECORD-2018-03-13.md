# MySQL 学习笔记

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

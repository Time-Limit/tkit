# MySQL 学习笔记

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

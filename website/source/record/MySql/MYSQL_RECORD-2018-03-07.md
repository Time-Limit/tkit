# MySQL 学习笔记

## 2018-03-07

### 嵌套子查询

```
SELECT tn.cnx, tn.cny FORM tn WHERE tn.cny IN (SELECT tn1.cnz FROM tn1);

SELECT tn.cnx, tn.cny, (SELECT COUNT(*) FROM tn1) AS cnt FROM tn;
```

当子查询结果集用于父查询的WHERE时，应该只包含一列。
当子查询结果集合是父查询结果集的一部分时，应该只有一行一列。

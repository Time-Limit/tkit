# MySQL 学习笔记

## 2018-03-06

### GROUP BY 和 HAVING

在 SELECT 语句中， 可以用 GROUP BY 对结果集进行分组，用 HAING 对结果集进行过滤。
```
SELECT CONCAT(cnx, '(', cny, ')') AS title, COUNT(cnz) AS cnt FROM tn GROUP BY title HAVING cnt > 4 ORDER BY cnt DESC;
```

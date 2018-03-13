# MySQL 学习笔记

## 2018-03-09

### UNION

* 合并多条 SELECT 语句的查询结果。
* UNION 合并的每个查询必须具有相同的列，表达式或聚集函数。
* 列类型必须兼容。
* UNION 会过滤重复记录。UNION ALL 会保留所有记录。
* 只允许有一个 ORDER_BY 在 最后一个 SELECT 后面。这个 ORDER_BY 会排序所有的结果。

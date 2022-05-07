# Decouple Your DBMS

![20220507103647](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220507103647.png)

把上层做成无状态的，并发控制等信息下沉到存储层。schema，shard等元信息存在单独的metadata server中

关键是理清不同的DSL他们的执行流程
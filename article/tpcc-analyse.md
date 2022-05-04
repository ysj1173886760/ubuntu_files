# TPCC 解析

![20220504091256](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220504091256.png)

Warehouse的数量将决定其他表的大小

![20220504091358](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220504091358.png)

* The numbers next to the relationship arrows represent the cardinality of the relationships (average number of children per parent).
* The plus (+) symbol is used after the cardinality of a relationship or table to illustrate that this number is subject to small variations in the initial database population over the measurement interval as rows are added or deleted.

![20220504092155](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220504092155.png)

最大情况下New-Order占45%

New-Order和Payment共接近90%，所以主要考虑这两种事务类型

分析一下CockroachDB的tpcc测试脚本

https://github.com/cockroachdb/cockroach/blob/master/pkg/workload/tpcc/new_order.go

最开始一段是生成本次NewOrder的信息，比如购买物品的ID，是否涉及到远端仓库

```go
d := newOrderData{
    wID:    wID,
    dID:    int(randInt(rng, 1, 10)),
    cID:    n.config.randCustomerID(rng),
    oOlCnt: int(randInt(rng, 5, 15)),
}
```

然后开始一个事务

首先通过district得到当前order的id，同时查询district tax（一次Update + Select)

```go
// Select the district tax rate and next available order number, bumping it.
n.updateDistrict = n.sr.Define(`
    UPDATE district
    SET d_next_o_id = d_next_o_id + 1
    WHERE d_w_id = $1 AND d_id = $2
    RETURNING d_tax, d_next_o_id`,
)
```

查询warehouse tax（一次Select）

```go
// Select the warehouse tax rate.
n.selectWarehouseTax = n.sr.Define(`
    SELECT w_tax FROM warehouse WHERE w_id = $1`,
)
```

查询本次订单的所有item的信息（一次Select）

```go
for i, item := range d.items {
    itemIDs[i] = fmt.Sprint(item.olIID)
}
rows, err := tx.Query(
    ctx,
    fmt.Sprintf(`
        SELECT i_price, i_name, i_data
        FROM item
        WHERE i_id IN (%[1]s)
        ORDER BY i_id`,
        strings.Join(itemIDs, ", "),
    ),
)
```

从仓库中查询item的存量（一次Select）

```go
for i, item := range d.items {
    stockIDs[i] = fmt.Sprintf("(%d, %d)", item.olIID, item.olSupplyWID)
}
rows, err = tx.Query(
    ctx,
    fmt.Sprintf(`
        SELECT s_quantity, s_ytd, s_order_cnt, s_remote_cnt, s_data, s_dist_%02[1]d
        FROM stock
        WHERE (s_i_id, s_w_id) IN (%[2]s)
        ORDER BY s_i_id`,
        d.dID, strings.Join(stockIDs, ", "),
    ),
)
```

插入order以及new order（两个插入）

```go
n.insertOrder = n.sr.Define(`
    INSERT INTO "order" (o_id, o_d_id, o_w_id, o_c_id, o_entry_d, o_ol_cnt, o_all_local)
    VALUES ($1, $2, $3, $4, $5, $6, $7)`,
)

n.insertNewOrder = n.sr.Define(`
    INSERT INTO new_order (no_o_id, no_d_id, no_w_id)
    VALUES ($1, $2, $3)`,
)

```

更新stock的数量（一次Update，更新若干行）

```go
fmt.Sprintf(`
    UPDATE stock
    SET
        s_quantity = CASE (s_i_id, s_w_id) %[1]s ELSE crdb_internal.force_error('', 'unknown case') END,
        s_ytd = CASE (s_i_id, s_w_id) %[2]s END,
        s_order_cnt = CASE (s_i_id, s_w_id) %[3]s END,
        s_remote_cnt = CASE (s_i_id, s_w_id) %[4]s END
    WHERE (s_i_id, s_w_id) IN (%[5]s)`,
    strings.Join(sQuantityUpdateCases, " "),
    strings.Join(sYtdUpdateCases, " "),
    strings.Join(sOrderCntUpdateCases, " "),
    strings.Join(sRemoteCntUpdateCases, " "),
    strings.Join(stockIDs, ", "),
)
```

最后插入new order（一次Insert，插入若干行）
```go
fmt.Sprintf(`
    INSERT INTO order_line(ol_o_id, ol_d_id, ol_w_id, ol_number, ol_i_id, ol_supply_w_id, ol_quantity, ol_amount, ol_dist_info)
    VALUES %s`,
    strings.Join(olValsStrings, ", "),
),
```

结束事务

可以看到这个过程中涉及到大量的应用端与数据库端的交互

这个可以通过用“存储过程”来避免（有代价）

NewOrder的事务中，需要涉及到和远端同步的，也就是更新操作，一共有5次：
1. 更新district的next order id（单行单属性）
2. 插入order（插入一行）
3. 插入new order（插入一行）
4. 更新stock（更新5～15行）
5. 插入order line（插入5～15行）

每次都需要在SQL语句中附带整个需要更新的数据


再看Payment

更新warehouse balance，并返回warehouse的信息（Update + Select）

```go
// Update warehouse with payment
p.updateWarehouse = p.sr.Define(`
    UPDATE warehouse
    SET w_ytd = w_ytd + $1
    WHERE w_id = $2
    RETURNING w_name, w_street_1, w_street_2, w_city, w_state, w_zip`,
)
```

更新district balance，并返回district的信息（Update + Select）

```go
// Update district with payment
p.updateDistrict = p.sr.Define(`
    UPDATE district
    SET d_ytd = d_ytd + $1
    WHERE d_w_id = $2 AND d_id = $3
    RETURNING d_name, d_street_1, d_street_2, d_city, d_state, d_zip`,
)
```

更新customer的信息

```go
// Update customer with payment.
// If the customer has bad credit, update the customer's C_DATA and return
// the first 200 characters of it, which is supposed to get displayed by
// the terminal. See 2.5.3.3 and 2.5.2.2.
p.updateWithPayment = p.sr.Define(`
    UPDATE customer
    SET (c_balance, c_ytd_payment, c_payment_cnt, c_data) =
    (c_balance - ($1:::float)::decimal, c_ytd_payment + ($1:::float)::decimal, c_payment_cnt + 1,
            case c_credit when 'BC' then
            left(c_id::text || c_d_id::text || c_w_id::text || ($5:::int)::text || ($6:::int)::text || ($1:::float)::text || c_data, 500)
            else c_data end)
    WHERE c_w_id = $2 AND c_d_id = $3 AND c_id = $4
    RETURNING c_first, c_middle, c_last, c_street_1, c_street_2,
                c_city, c_state, c_zip, c_phone, c_since, c_credit,
                c_credit_lim, c_discount, c_balance, case c_credit when 'BC' then left(c_data, 200) else '' end`,
)
```

这里有一个比较关键的点，当c_credit为'BC'时，我们会更新c_data。这里的更新逻辑是由SQL处理的。SQL语句内部没有after-image，而是一个逻辑的Append。

最后是一个insert history（一次Insert）

```go
p.insertHistory = p.sr.Define(`
    INSERT INTO history (h_c_id, h_c_d_id, h_c_w_id, h_d_id, h_w_id, h_amount, h_date, h_data)
    VALUES ($1, $2, $3, $4, $5, $6, $7, $8)`,
)
```

其实只有customer的更新这里，传输SQL和after-image出现了差别。原因是这个string append的操作不是由应用端处理，而是数据库处理的。

通过上面的两种类型的transaction可以发现大多数情况下SQL里面就包含了本次的数据，所以这时候我们同步的时候传输SQL和传输写集是类似的。然而当逻辑由数据库处理的时候，也就是写集的生成在数据库段而非在应用端的时候，两种策略就会出现不同的表现。

一个insight：我们可以特殊处理那些可能涉及到较大after-image的项。具体的就是长string/text/varchar类型的数据，我们可以传逻辑操作而非具体的image。

![20220504104220](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220504104220.png)

其中Parser/Planner/Executor可以是数据库现有的组件，也可以是我们自己写的。比如我们可以只是传输必要的逻辑操作，而不是整个SQL语句

一个直观的感觉就是传递写集可以避免数据库前端的开销，并且更加简单。然后我们可以通过传输逻辑操作的方式来避免某些大属性传递具体数据的开销。

这样我们就可以拆分开一个事务的数据：

部分数据可以是Physical data，即传递属性ID，RowID，after-image，timestamp。也可以是Logical data，即传递属性ID，RowID，Operation（比如"Append, data")，timestamp

但是对于更复杂的语句我们还是需要传SQL来得到最终的写集，即需要更多的语义信息。但是对于TPCC这样的来说行级别的逻辑操作就够用

有点像是嵌入到数据库内部的中间件？

Hybrid Physical Data/Logical Operation

challenge：
1. 实现困难，比如传递的logical operation也要保证不能太大，并且要考虑的情况较多，不仅仅是append。因为SQL能提供的语义很多
2. 一定程度的加大了耦合性，因为在决策传递物理数据和逻辑操作的时候需要知道语义信息（transaction的page model和object model)


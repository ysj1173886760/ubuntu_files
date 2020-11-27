## mt19937

[参考文章mt19937](https://blog.csdn.net/caimouse/article/details/55668071)

[参考文章uniform_int_distribution](https://blog.csdn.net/Andyooper/article/details/893146780)

[参考文章random_device](https://www.cnblogs.com/egmkang/archive/2012/09/06/2673253.html)

mt19937是C++11中新加入的特性。

Mersenne Twister算法译为马特赛特旋转演算法，是伪随机数发生器之一，其主要作用是生成伪随机数。

Mersenne Twister有以下优点：随机性好，在计算机上容易实现，占用内存较少(mt19937的C程式码执行仅需624个字的工作区域)，与其它已使用的伪随机数发生器相比，产生随机数的速度快、周期长，可达到**2^19937-1**，且具有623维均匀分布的性质，对于一般的应用来说，足够大了，序列关联比较小，能通过很多随机性测试。

在boost库中有相关的实现

使用方法

```c++
mt19937 rand_num(seed); //先创建mt19937对象，构造函数的参数为随机数种子
//可以直接调用rand_num来获得随机数
//如果需要范围，使用uniform_int_distribution<type> _name(lower_bound, upper_bound) 来确定范围
//最后再调用_name(rand_num)来获得随机数
```

![image-20201109123800407](/home/sheep/.config/Typora/typora-user-images/image-20201109123800407.png)

其中我们使用mt19937作为uniform_int_distribution的随机引擎

​	![image-20201109124301475](/home/sheep/.config/Typora/typora-user-images/image-20201109124301475.png)

要用random_device先创建一个对象，再用对象来生成随机数

![image-20201109124421681](/home/sheep/.config/Typora/typora-user-images/image-20201109124421681.png)

## unordered_map以pair做键

[参考文章](https://blog.csdn.net/qq_32439305/article/details/107344082)

出于性能考虑，C++把对pair的hash函数删去了，所以在用pair作为hashtable的键的时候，hash函数需要自己写

这里给出一个泛型的版本

```c++
template<class T1, class T2> 
struct pair_hash//没这个pair 就不能在unorder——map快乐的玩耍了
{
    size_t operator() (const pair<T1, T2>& p) const
    {
        return hash<T1>()(p.first) ^ hash<T2>()( p.second);//异或思想
    }
    //如果遇到了<3,5>和<5,3>怎么办，皮神有想法，再把两个hash判断一遍
    
    bool operator()(const pair<T1,T2> &lhs, const pair<T1,T2> &rhs) const
    {
        return equal_to<T1>()(lhs.first,rhs.first) && equal_to<T2>()(lhs.second,rhs.second);
    }
};
```

使用方法

```c++
unordered_map<pair<int,int>,double,pair_hash<int,int>,pair_hash<int,int>> minDis;
minDis.insert({{1,2}, 1.5});
```

但是会用了，还不明白原理。查阅[cppreference](http://www.cplusplus.com/reference/unordered_map/unordered_map/)得到unordered_map的定义

```c++
template < class Key,                                    // unordered_map::key_type
           class T,                                      // unordered_map::mapped_type
           class Hash = hash<Key>,                       // unordered_map::hasher
           class Pred = equal_to<Key>,                   // unordered_map::key_equal
           class Alloc = allocator< pair<const Key,T> >  // unordered_map::allocator_type
           > class unordered_map;
```

有关后两个参数，在cppreference中有很清晰的解释

Hash

A unary function object type that takes an object of type *key type* as argument and returns a unique value of type [size_t](http://www.cplusplus.com/size_t) based on it. This can either be a class implementing a *function call operator* or a pointer to a function (see [constructor](http://www.cplusplus.com/unordered_map::unordered_map) for an example). This defaults to `hash<Key>`, which returns a hash value with a probability of collision approaching `1.0/std::numeric_limits<size_t>::max()`.
The `unordered_map` object uses the hash values returned by this function to organize its elements internally, speeding up the process of locating individual elements.
Aliased as member type `unordered_map::hasher`.

Pred

A binary predicate that takes two arguments of the *key type* and returns a `bool`. The expression `pred(a,b)`, where *pred* is an object of this type and *a* and *b* are key values, shall return `true` if *a* is to be considered equivalent to *b*. This can either be a class implementing a *function call operator* or a pointer to a function (see [constructor](http://www.cplusplus.com/unordered_map::unordered_map) for an example). This defaults to `equal_to<Key>`, which returns the same as applying the *equal-to operator* (`a==b`).
The `unordered_map` object uses this expression to determine whether two element keys are equivalent. No two elements in an `unordered_map` container can have keys that yield `true` using this predicate.
Aliased as member type `unordered_map::key_equal`.

所以看代码中的实现，只有第一个参数和第二个参数都相同时，hashtable才会把他们判断为相同的元素，否则对于`<3, 5> <5, 3>`这种数据就会判断为hash collision，虽然他们的hash值相同（亦或满足交换律），但是会被认定为不同的元素

## 一笔画问题

[参考文章](https://leetcode-cn.com/problems/reconstruct-itinerary/solution/zhong-xin-an-pai-xing-cheng-by-leetcode-solution/)

给出相关的定义：

- 通过图中所有边恰好一次且行遍所有顶点的通路称为欧拉通路。
- 通过图中所有边恰好一次且行遍所有顶点的回路称为欧拉回路。
- 具有欧拉回路的无向图称为欧拉图。
- 具有欧拉通路但不具有欧拉回路的无向图称为半欧拉图。

那么怎么判断这个图是不是欧拉图或者半欧拉图呢？

- 对于无向图 G，G 是欧拉图当且仅当 G 是连通的且没有奇度顶点。
- 对于无向图 G，G 是半欧拉图当且仅当 G 是连通的且 G 中恰有 2 个奇度顶点。
- 对于有向图 G，G 是欧拉图当且仅当 G 的所有顶点属于同一个强连通分量且每个顶点的入度和出度相同。

- 对于有向图 G，G 是半欧拉图当且仅当 G 的所有顶点属于同一个强连通分量且
  - 恰有一个顶点的出度与入度差为 1；
  - 恰有一个顶点的入度与出度差为 1；
  - 所有其他顶点的入度和出度相同。

Hierholzer 算法用于在连通图中寻找欧拉路径，其流程如下：

1. 从起点出发，进行深度优先搜索。
2. 每次沿着某条边从某个顶点移动到另外一个顶点的时候，都需要删除这条边。
3. 如果没有可移动的路径，则将所在节点加入到栈中，并返回。

最终依次弹出栈得到的序列就是欧拉路

对于正确性，可以简单的说一下思路，欧拉路径中的最后一个节点一定是没路可走的节点，即边都删完了，我们把序列中的这最后一个节点删去，注意是删边，那么就会出现序列中的新一个最后一个节点，我们依次删除这些节点对应的边，就是欧拉路径了。

也可以这样考虑，如果欧拉路径中的一个节点在另一个节点的后面，那么一定存在一条路径是从一个节点通向另一个节点的，那么在dfs过程中就必定在前面的节点之前将后面的节点入栈。

当然不是严格的证明，不过可以通过这个思路把算法记下来

## 比较函数在比较相同元素时返回false

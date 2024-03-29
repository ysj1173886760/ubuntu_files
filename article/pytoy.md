### PyToy review

核心就是用Node来抽象每个算子，然后根据他们的先后顺序将这些Node组合成一个图

计算的时候用pull或者拓扑排序的方法都可以

Node里设置了抽象方法`compute`和`get_graident`，即计算对应的前向传播的值和反向传播的值

Variable就是一种Node，类似于tensorflow中的placeholder，用来存储数据的

计算图的起始节点都要是variable

我这里做了简化，并没有区分常量和变量，而是统一作为variable使用的

第二大核心就是Ops了，也就是Operator

Operator也是一种Node，但是没有做任何实现

所有的真正的operator都继承自Operator，主要是为了快速判断类型用的

然后就根据需求要什么Operator就实现什么就可以了

在初始化Node的时候，我就会把他的父节点传入进去，构建父子节点的边

这里的conv和pooling都用了img2col做优化

然后计算图的最终节点一般都会是一个loss，用来计算损失函数的

也有一个抽象类LossFunction

Loss和其他operator不同的就是，其他的operator需要先计算他们子节点的graident，才能计算他们自己的

而loss自己就是导出最初的graident的地方，所以他不需要child的值，当然他也没有child

和MatrixFlow不同的是，我这里计算graident的时候不是求jacob矩阵，而是在求graident的时候传入他的子节点，表示对哪个子节点求梯度

每个节点的graident就是他的top_diff，也就是上层传给他的梯度，然后我们根据子节点是谁来求对应的bottom_diff

基本上计算图的部分就结束了，把这块实现出来就已经可以跑数据了

下一个是优化器，优化器用来控制整个的计算图的更新

Optimizer记录了整个计算图的可训练节点数量，以及要优化的target

每个step都会清空上一次的graident，然后在target上进行forward，最后再求出所有trainable node上的梯度，然后根据优化器的算法来更新值

Layer里面就是封装了常用的算子，让我们可以通过层的视角来堆叠网络，但是内部还是计算图

比如Conv层，就封装了一个conv算子和一个add算子，然后对应的conv的weight和add的bias就都会在这个layer中创建好

因为我们在上层只关心的是输入的数据和输出的结果，并不会在意weight这些变量，所以就通过layer把他们隐藏起来

Trainer则是用来帮我们封装训练细节的

比如处理batch的输入数据，调用优化器什么的

我在trainer中特殊优化了一些操作。具体来说，trainer会首先找到我们这次计算需要的节点，其他的节点就会被mark为失效状态

然后计算出度和入度，可以让我们进行toposort

然后根据拓扑序计算值，这样的好处就是当一个节点计算结束后，我们可以释放这个节点的空间，通过这个方法我们可以节省需要的显存。具体的数据在dev log中有写

对于分布式训练，其实思路很简单。就是每个batch都把自己梯度发上去，然后PS帮我们做一下平均再发回来

但是有几个要注意的点，一个就是对于push参数和pull参数要做同步

比如有一台服务器已经push到第二轮参数了，第一轮的参数还没有被pull完，他就应该阻塞住

还有一个问题就是bn中的参数，虽然bn并不是一个trainable的Node，但是他里面含有需要同步的参数。

所以我们也要同步bn中的running_var_x和running_var_mean。否则虽然graident相同，但是由于bn的修正，也会导致最终测试结果出现错误

然后在trainer中封装分布式训练的这些操作，就可以对上层隐藏分布式训练的这些细节
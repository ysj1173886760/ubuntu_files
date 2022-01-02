### 通过链表理解Rust中的Ownership

我们通过[leetcode](https://leetcode-cn.com/problems/merge-two-sorted-lists/)中的一道链表题来理解rust中的链表

```rust
#[derive(PartialEq, Eq, Clone, Debug)]
pub struct ListNode {
    pub val: i32,
    pub next: Option<Box<ListNode>>
}
```

leetcode中rust的链表定义是这样的

可以看到，我们有clone的属性，但没有copy的属性，也就是说我们在赋值的时候的语义是移动，而非复制

而根据结构体的定义来看，next并非引用，说明一个节点也拥有他的后继节点的所有权，即ownership

所以在进行drop，clone等操作的时候，rust会使用深拷贝（深free）

```rust
impl Solution {
    pub fn merge_two_lists(list1: Option<Box<ListNode>>, list2: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut dummy_head = Box::new(ListNode::new(0));
        let mut head = &mut dummy_head;
        let (mut l1, mut l2) = (list1, list2);

        while let (Some(lhs), Some(rhs)) = (l1.as_ref(), l2.as_ref()) {
            if lhs.val <= rhs.val {
                head.next = l1;
                head = head.next.as_mut().unwrap();
                l1 = head.next.take();
            } else {
                head.next = l2;
                head = head.next.as_mut().unwrap();
                l2 = head.next.take();
            }
        }

        head.next = if l1.is_some() { l1 } else { l2 };
        dummy_head.next
    }
}
```

然后我们看这道题的代码

首先是经典的dummy_head，这里不需要去用option来封装我们的dummy_head，因为他是确定存在的

然后我们再用一个可变引用去引用dummy_head，这就是我们处理链表合并操作时候的那个指针

这个可变的借用就相当于是一个拥有更改权限的指针，指向了原本dummy_head的内存区域

然后我们再将传入的参数重新shadow为可变的形式，这时候list1和list2绑定的对象就被移动到了l1和l2中，那么他们本身的绑定也就失效了

然后我们用只读借用绑定到lhs和rhs中，这时候注意我们用了`as_ref`，是一个只读的引用，所以l1和l2本身并不会失效。lhs和rhs就只是对原对象的一个`const reference`

比较值之后，在把l1赋给了head.next

这里的语义就是移动，所以l1以及他的后继节点的所有权都被转移到了head.next之中

此时l1的绑定失效

然后我们移动head，让head重新指向他的下一个节点

在C++中，我们一般会这么写`head = head.next`

但是在rust中，我们需要显式的声明可变引用，而且由于head是对指针的可变引用，而不是option的可变引用，所以我们还需要进行一次unwrap

在这里可以发现，对于mutable reference来说，这里的写法是类似的

但是注意，这里我们是把整个的l1都移动过去了，但是实际上我们只想移动当前的这一个节点，所以我们还需要把后面的那部分给拿回来

最后这一步就是将后面的节点给拿回来，但是head是一个可变的引用。在可变引用的时候，rust需要我们显式的指出我们是希望是使用移动语义

所以在这里，我们用take拿到head.next本身的值（不使用take得到的则是一个引用），并移动给l1，这时候l1的绑定则重新生效

```rust
impl Solution {
    pub fn merge_two_lists(list1: Option<Box<ListNode>>, list2: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        match (list1, list2) {
            (Some(l1), None) => Some(l1),
            (None, Some(l2)) => Some(l2),
            (Some(mut l1), Some(mut l2)) => {
                if l1.val < l2.val {
                    let nxt = l1.next.take();
                    l1.next = Self::merge_two_lists(nxt, Some(l2));
                    Some(l1)
                } else {
                    let nxt = l2.next.take();
                    l2.next = Self::merge_two_lists(Some(l1), nxt);
                    Some(l2)
                }
            },
            _ => None
        }
    }
}
```

这里是一个递归版的实现，我感觉实现的还是比较简洁的

这里直接match给出的两个参数，然后对于都为非空的情况下，将对象移动到l1和l2中

然后nxt则是拿掉l1整个的后继节点，此时l1的next就会变成None

然后我们用后继节点和l2递归的进行merge，返回值就是节点的head，我们再将其移动到l1中，让l1获取这个head的所有权

最后返回l1，就完成了一次合并

这里对于所有权的移动非常巧妙，需要多加理解

[参考文章](https://zhuanlan.zhihu.com/p/84953434)



# csappp_lab1
## 题解   
1.bitxor  
题目描述：
>bitXor - x^y using only ~ and &   
>Example: bitXor(4, 5) = 1    
>Legal ops: ~ &    
>Max ops: 14    
>Rating: 1  

解题思路：   
利用～和&生成^，简单拆分异或即可，并运用德尔摩根定律    
代码：   
```
int bitXor(int x, int y)
{
  return ~(~(x & ~y) & ~(~x & y));
}
```  
2.tmin  
题目描述：  
>tmin - return minimum two's complement integer   
>Legal ops: ! ~ & ^ | + << >>    
>Max ops: 4   
>Rating: 1     

解题思路：    
32位最小补码整数即0x80000000  
代码：   
```
int tmin(void)
{
  return 1 << 31;
}
```    
3.isTmax
题目描述：  
>isTmax - returns 1 if x is the maximum,two's complement number, and 0 otherwise          
>Legal ops: ! ~ & ^ | +     
>Max ops: 10    
>Rating: 1
    
解题思路：    
32位最大补码为0x70000000，然后我就不知道该怎么办了，根据题解x=Tmax时，x^(x+1)全为1，但当x=-1时也有这样的性质，所以要有方法加以判断，易知此时x+1=0，由此可以判断x=-1和x=Tmax。   
ps：位运算符(&,|,>>,<<等)是对位进行运算，结果仍然是运算数，而逻辑运算符如&&，！，｜｜，他们的结果为bool类型    
代码：   
```
int isTmax(int x) 
{
  int m = !(~((x + 1) ^ x));
  int n = !!(x+1);
  return m & n; 
}
```     
4.allOddBits  
题目描述：  
>allOddBits - return 1 if all odd-numbered bits in word set to where bits are numbered from 0 (least significant) to 31 (most significant)  >Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1  
>Legal ops: ! ~ & ^ | + << >>   
>Max ops: 12   
>Rating: 2
    
解题思路：    
本可以直接设置0xaaaaaaaa作为掩码检测奇数位，但禁用8bits的常数，故通过左移来生成需要的掩码    
代码：   
```
int allOddBits(int x)
{
  int a = 0xaa;
  int b = a << 8 | a;
  int c = b << 16 | b;
  return !(c ^ (x & c));
}
```    
5.negate  
题目描述：  
>negate - return -x   
>Example: negate(1) = -1.  
>Legal ops: ! ~ & ^ | + << >>  
>Max ops: 5  
>Rating: 2
    
解题思路：    
取反加一  
代码：   
```
int negate(int x)
{
  x = ~x + 1;
  return x;
}
```    
6.isAsciiDigit  
题目描述：  
>isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')   
>Example:isAsciiDigit(0x35) = 1.      
 > isAsciiDigit(0x3a) = 0. ​   
 > isAsciiDigit(0x05) = 0.   
>Legal ops: ! ~ & ^ | + << >>    
>Max ops: 15    
>Rating: 3

解题思路：    
0x30到0x37的位级表示位00110xxx，先忽略低3位设置掩码来判断，再特殊处理0x38，0x39
代码：   
```
int isAsciiDigit(int x)
{
  int a = !((x >> 3) ^ 0x06); //0x30<=x<=0x37
  int b = !(x ^ 0x38);
  int c = !(x ^ 0x39);
  return a | b | c;
}
```    
7.conditional  
题目描述：  
>iconditional - same as x ? y : z    
>Example: conditional(2,4,5) = 4    
>Legal ops: ! ~ & ^ | + << >>   
>Max ops: 16   
>Rating: 3

解题思路：    
利用ops达到条件表达式的效果，根据老板之前的提示条件语句
```
  if(a)
    b;
  else
    c;
```
 等价于表达式 $(a&b)|(!a&c)$ 其正确性我无法评价，但对于本题是无法得到正确结果的，在此再次强调位运算操作符和逻辑操作符的不同。对于此题，应根据x的值来设置对应的掩码，来设置返回y还是z的值。
 > $x!=0$|mask=0xffffffff   
 >-|-  
 > $x==0$|mask=0

代码：   
```
int conditional(int x, int y, int z)
{
  int mask = ((!!x) << 31) >> 31; 
  return ((mask & y) | (~mask & z)) ;
}
```    
8.isLessOrEqual  
题目描述：  
>isLessOrEqual - if x <= y then return 1, else return 0  
>Example: isLessOrEqual(4,5) = 1.    
>Legal ops: ! ~ & ^ | + << >>   
>Max ops: 24    
>Rating: 3     

解题思路：    
x<=y时，y-x的符号位为0，但需要注意y-x可能会溢出，但只有异号相减才会溢出，此时可以直接通过x，y的符号位来判断   
代码：   
```
int isLessOrEqual(int x, int y)
{
  int sx = (x >> 31) & 0x1;
  int sy = (y >> 31) & 0x1;
  int c1 = (~sx & sy);
  int c2 = (sx & ~sy);
  int y_x = (y + (~x + 1)) >> 31;
  return c1 | !c2 | !y_x ;
}
```    
9.LogicalNeg  
题目描述：  
>logicalNeg - implement the ! operator, using all of the legal operators except !       
>Examples: logicalNeg(3) = 0, logicalNeg(0) = 1     
>Legal ops: ~ & ^ | + << >>     
>Max ops: 12     
>Rating: 4   

解题思路：    
实现逻辑非关键在判断x是否为0，联想到之前leetcode刷题中异或的性质，直接让x与0异或来判断是否为0  
代码：   
```
int logicalNeg(int x)
{
  return !(x ^ 0);
}
```   
10. howMangBits  
题目描述：  
>howManyBits - return the minimum number of bits required to represent x in two's complement    
>Examples:howManyBits(12) = 5 ​     
 >howManyBits(298) = 10 ​      
 >howManyBits(-5) = 4 ​      
 >howManyBits(0) = 1 ​      
 >howManyBits(-1) = 1 ​      
 >howManyBits(0x80000000) = 32    
>Legal ops: ! ~ & ^ | + << >> 
>Max ops: 90    
>Rating: 4   
  
解题思路：    
对于正数需要的找到最左边的1，对于负数则按位取反后找最左边的1，题解采用二分查找，这是我完全没有想到的，二分竟然可以这么用，有空可以深入研究一下二分，之前leetcode上也频繁出现二分的题
代码：   
```
int howManyBits(int x)
{
  int b16,b8,b4,b2,b1,b0;
  int mask = x >> 31;
  x = (mask & ~x) | (~mask & x); //如果为正数，保持不变；如果为负数，按位取反

  //step1:判断高16为是否有1
  b16 = !!(x >> 16) << 4; //如果高16为有1,则b16 = 16，否则为0
  x >>= b16; //如果高16为有1,x右移16位舍弃低16位,在新的低16位继续查找；否则保持不变
  //step2:判断高8位是否有1
  b8 = !!(x >> 8) << 3;
  x >>= b8;
  //step3:高4位
  b4 = !!(x >> 4) << 2;
  x >>= b4;
  //step4:高2位
  b2 = !!(x >> 2) << 1;
  x >>= b2;
  //step5:高1位
  b1 = !!(x >> 1);
  x >>= b1;
  //step6:低1位
  b0 = x;

  return b16 + b8 + b4 + b2 + b1 + b0 + 1;
}
```   
11.floatScale2  
题目描述：  
>floatScale2 - Return bit-level equivalent of expression 2*f for      floating point argument f.   
> Both the argument and result are passed as unsigned int's, but they are to be interpreted as the bit-level representation of single-precision floating point values. 
>When argument is NaN, return argument    
>Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while >Max ops: 30    
>Rating: 4
 
解题思路：    
一开始没有理解题目的意思，现在看来说的就是无论是结果还是参数都是unsigned类型的，但是对他的底层二进制数按照ieee的浮点数解释 ，题目要求返回参数的两倍，根据ieee的浮点解释，考虑exp的三种情况，当exp全为1时，若frac为0，这时表示无穷大，两倍依旧是无穷大，若frac！=0，结果为NaN直接返回参数，当exp不全为0或1时，直接exp+1，当exp全为0时，这是我不理解的地方，然后有点懂了，此时不能直接使exp+1，只能通过左移frac1位达到目的，此题的关键就是分情况讨论
代码：   
```
unsigned floatScale2(unsigned uf)
{
  unsigned sign = (uf >> 31) & 0x1;
  unsigned exp = (uf >> 23) & 0xff;
  unsigned frac = uf & 0x7fffff;
  unsigned res;
  if (exp == 0xff)
  {
    res = uf;
  }
  else if (exp == 0)
  {
    frac <<= 1;
    res = (sign << 31) | (exp << 23) | frac;
  }
  else
  {
    exp++;
    res = (sign << 31) | (exp << 23) | frac;
  }
  return res;
}
```  
12.floatFloat2Int  
题目描述：  
>floatFloat2Int - Return bit-level equivalent of expression (int) f for floating point argument f.    
>Argument is passed as unsigned int, but it is to be interpreted as the bit-level representation of a single-precision floating point value.  
>Anything out of range (including NaN and infinity) should return 0x80000000u. 
>Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while  
>Max ops: 30    
>Rating: 4

解题思路：    
依旧分三种情况考虑，非规格的数即很接近0的数，转化为int后为0，特殊值时返回0x80000000u，规格数超过int范围就返回0x80000000u，否则就转换   

>在将normalized float转换为int型数时，
>* M = 1.frac如果E >= 31，小数点右移31位，此时隐含的1和frac占32位，另外还需要一个符号位，超出了int型范围
>* 如果E < 0，小数点左移1位后为0.1frac，转换为int后为0
>* 如果0 < E < 23, 小数点左移E为后需要舍弃frac中部分位，此时直接将frac右移23-E位，抹去小数部分
>* 如果23 <= E < 31，此时小数点右移后frac全部移到小数点以左，将frac左移E-23位，在后面补零

代码：   
```
int floatFloat2Int(unsigned uf)
{
  int sign = (uf >> 31) & 0x1;
  int exp = (uf >> 23) & 0xff;
  int frac = uf & 0x7fffff;
  int e = exp - 127;
  if (e < 0)
  {
    return 0;
  }
  else if (e >= 31)
  {
    return 0x80000000u;
  }
  else
  {
    frac = frac | (1 << 23);
    if (e < 23)
    {
      frac <<= (23 - e);
    }
    else
    {
      frac <<= (e - 23);
    }
  }
  if (sign)
  {
    return -frac;
  }
  else
  {
    return frac;
  }
}
```  
13.floatPower2  
题目描述：  
>floatPower2 - Return bit-level equivalent of the expression 2.0^x (2.0 raised to the power x) for any 32-bit integer x.     
>The unsigned value that is returned should have the identical bit representation as the single-precision floating-point number 2.0^x.    
>If the result is too small to be represented as a denorm, return 0.     
>If too large, return +INF.    
>Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while  
>Max ops: 30  
>Rating: 4

解题思路：   
>根据浮点数求值公式v = (-1)^s * M * 2 ^ E = 2 ^ x
>1. normalized​   令M = 1(frac = 0)，x = E =  exp - Bias, exp = x + Bias
>2. denormalized​   exp = 0，在frac中令某一位为1，从而可以使x更小

>下面是对边界情况的分析   
    >1. denormalized当frac = 100 0000 0000 0000 0000 0000时，M = 0.1b = 0.5, E = 1- Bias = -126，此时v = 0.5 * 2.0 ^ -126 = 2.0 ^ -127,当frac = 000 0000 0000 0000 0000 0001时，M = 0.000 0000 0000 0000 0000 0001 = 2.0 ^ -22, E = -126，此时v = 2.0 ^ -22 * 2 ^ -126 = 2.0 ^ -148  
    >2. normalizedexp = 0xFF时，E = exp - Bias = 127exp = 1时，E = exp - Bias = -126


代码：   
```
unsigned floatPower2(int x) {
  if (x > 127) //too large, return +INF
  {
    return (0xFF << 23);
  }
  else if (x < -148) //too small, return 0
  {
    return 0;
  }
  else if (x >= -126) //norm，计算exp
  {
    int exp = x + 127;
    return (exp << 23);
  }
  else //denorm，令frac中某一位为1
  {
    int t = 148 + x;
    return (1 << t);
  }

}
``` 
## 总结  
大量借鉴引用[这里](https://zhuanlan.zhihu.com/p/137284416)   
好好学习就完事了，对这吊毛浮点数的理解又加深了


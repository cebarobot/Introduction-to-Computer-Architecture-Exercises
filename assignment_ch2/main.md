# 计算机体系结构基础 - 第 2 章作业

## 2.1
列出一种指令系统的不同运行级别之间的关系。

**解**

如图，为 MIPS 指令系统不同运行级别的关系图。

<img src="./图片1.png" style="width: 25em;">

## 2.2
用 C 语言描述段页式存储管理的地址转换过程。

**解**

C 语言的描述如下：
```c
Page_table * page_base = segment_table[segment];
void * phys_addr = offset + page_base[virtual_page] << PAGE_SIZE;
```

## 2.3
请简述桌面电脑 PPT 翻页过程中用户态和核心态的转换过程。

**解**

当按下键盘后，处理器接受到中断信号，从用户态切换到核心态以响应中断，随后回到用户态运行 PowerPoint；PowerPoint 调用显示驱动程序，处理器进入核心态以执行显示操作，随后回到用户态。

## 2.4
给定下列程序片段：
```plain
A = B + C
B = A + C
C = B + A
```
（1）写出上述程序片段在四种指令系统类型（堆栈型、累加器型、寄存器-存储器型、寄存器-寄存器型）中的指令序列。  
（2）假设四种指令系统都属于 CISC 型，令指令码宽度为 $x$ 位，寄存器操作数宽度为 $y$ 位，内存地址操作数宽度为 $z$ 位，数据宽度为 $w$ 位。分析指令的总位数和所有内存访问的总位数。  
（3）微处理器由 32 位时代进入 64 位时代，上述四种指令系统类型哪种更好？

**解**

（1）
<table>
  <thead>
    <tr>
      <th style="text-align:center; width:8em;">堆栈型</th>
      <th style="text-align:center; width:8em;">累加器型</th>
      <th style="text-align:center; width:10em;">寄存器-存储器型</th>
      <th style="text-align:center; width:12em;">寄存器-寄存器型</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td style="text-align:left">
<pre><code>PUSH  B
PUSH  C
ADD
POP   A
PUSH  A
PUSH  C
ADD
POP   B
PUSH  B
PUSH  A
ADD
POP   C
</code></pre>
      </td>
      <td style="text-align:left">
<pre><code>LOAD  B
ADD   C
STORE A
ADD   C
STORE B
ADD   A
STORE C
</code></pre>
      </td>
      <td style="text-align:left">
<pre><code>LOAD  R1, B
ADD   R1, C
STORE A, R1
ADD   R1, C
STORE B, R1
ADD   R1, A
STORE C, R1
</code></pre>
      </td>
      <td style="text-align:left">
<pre><code>LOAD  R2, B
LOAD  R3, C
ADD   R1, R2, R3
STORE A, R1
ADD   R2, R1, R3
STORE B, R2
ADD   R3, R1, R2
STORE C, R3
</code></pre>
      </td>
    </tr>
  </tbody>
</table>

（2）

对于堆栈型：
* 其指令总位数为 $9 \times (x + z) + 3 \times x = 12x + 9z$；
* 其访存总位数为 $(12x + 9z) + 9 \times w = 12x + 9z + 9w$。

对于累加器型：
* 其指令总位数为 $7 \times (x + z) = 7x + 7z$；
* 其访存总位数为 $(7x + 7z) + 7 \times w = 7x + 7z + 7w$。

对于寄存器-存储器型：
* 其指令总位数为 $7 \times (x + y + z) = 7x + 7y + 7z$；
* 其访存总位数为 $(7x + 7y + 7z) + 7 \times w = 7x + 7y + 7z + 7w$。

对于寄存器-寄存器型：
* 其指令总位数为 $5 \times (x + y + z) + 3 \times (x + 3y) = 8x + 14y + 5z$；
* 其访存总位数为 $(8x + 14y + 5z) + 5 \times w = 8x + 14y + 5z + 5w$。

（3）微处理器由 32 位时代进入 64 位时代，寄存器-寄存器型指令系统更好。其访问速度快，便于编译器调度优化，容易判断相关性，容易实现流水线、多发射、乱序执行等。


## 2.5
写出 `0xDEADBEEF` 在大尾端和小尾端下在内存中的排列（由地址 `0` 开始）。

**解**

大尾端：
```
0   1   2   3
DE  AD  BE  EF
```

小尾端：
```
0   1   2   3
EF  BE  AD  DE
```

## 2.6
在你的机器上编写 C 程序来得到不同数据类型占用的字节数，给出程序和结果。

**解**

C 程序如下：
```c
#include <stdio.h>
int main() {
    printf("char            %ld\n", sizeof(char));
    printf("short           %ld\n", sizeof(short));
    printf("int             %ld\n", sizeof(int));
    printf("long            %ld\n", sizeof(long));
    printf("long long       %ld\n", sizeof(long long));
    printf("float           %ld\n", sizeof(float));
    printf("double          %ld\n", sizeof(double));
    printf("long double     %ld\n", sizeof(long double));
    return 0;
}
```

程序输出结果如下：
```
char            1
short           2
int             4
long            8
long long       8
float           4
double          8
long double     16
```

即

| C 语言类型    | 占用字节数 |
| :------------ | :--------: |
| `char`        |     1      |
| `short`       |     2      |
| `int`         |     4      |
| `long`        |     8      |
| `long long`   |     8      |
| `float`       |     4      |
| `double`      |     8      |
| `long double` |     16     |

## 2.7
根据 MIPS 指令集的编码格式计算条件转移指令和直接转移指令的跳转范围。

**解**

对于条件转移指令，其采用 I-type 指令，立即数长度为 $16$ 位，实际偏移量为立即数左移 $2$ 位，且为有符号数。故其跳转范围是相对于当前 PC，$-2^{17} \sim 2^{17}$ 字节，即 $\pm 128\ \text{KB}$。

对于直接转移指令，其采用 J-type 指令，立即数长度位 $26$ 位，实际偏移量为立即数左移 $2$ 位。故其跳转范围是 $2^{28}$ 字节，即 $256\ \text{MB}$。

## 2.8
不使用 LWL 和 LWR，写出如图 2.10 的不对齐加载（小尾端）。

**解**

小尾端情况下，等效代替 `LWR R1, 1`、`LWL R1, 4` 的代码如下：
```
LW    R1, 0
SRL   R1, 8
LW    R2, 4
SLL   R2, 24
OR    R1, R1, R2
```

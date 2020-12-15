# 计算机体系结构基础 - 第 11 章作业

## 11.1
关于多核处理器的 Cache 结构，请介绍 UCA 与 NUCA 的特点。

**解**

* UCA 是一种集中式共享结构，处理器核通过总线、交叉开关连接 LLC（Last Level Cache）。UCA 中，所有处理器核对 LLC 的访问延迟相同，常用于核数较少的多核处理器。UCA 结构的集中式 Cache 的可扩展性受到总线、交叉开关的限制，容易在处理器核数量增加时成为性能瓶颈。
* NUCA 是一种分布式共享结构，每个处理器核拥有本地的 LLC，并通过片上互连访问其他处理器核的 LLC。NUCA 中吗，处理器核对不同位置的 LLC 访问延迟不同，常用于较多核数的多核、众核处理器。NUCA 结构需要高校的 Cache 查找和替换算法，以提升性能。NUCA 结构通常采用可扩展性的片上互连，采用基于目录的 Cache 一致性协议，具有良好的扩展性。

## 11.2
有两个并行执行的线程，在顺序一致性和弱一致性下，它各有几种正确的执行顺序？给出执行次序和最后的正确结果（假设 `X`、`Y` 的初始值均为 0。
```
P1                  P2
X = 1;              Y = 1;
print Y;            print X;
```

**解**

顺序一致性的正确执行次序有下面 6 种：
```
1:                  2:                  3:
    X = 1;              X = 1;              X = 1;
    Print Y;            Y = 1;              Y = 1;
    Y = 1;              print Y;            print X;
    Print X;            print X;            print Y;
R: X = 1, Y = 0     R: X = 1, Y = 1     R: X = 1, Y = 1

4:                  5:                  6:
    Y = 1;              Y = 1;              Y = 1;
    X = 1;              X = 1;              print X;
    print Y;            print X;            X = 1;
    print X;            print Y;            print Y;
R: X = 1, Y = 1     R: X = 1, Y = 1     R: X = 0, Y = 1
```

对于弱一致性：
* 如果认为 `print X;` 和 `print Y;` 具有同步操作的特点，那么顺序一致性的 2、3、4、5 是可能的正确执行次序。
* 如果认为程序中没有同步，那么上述 6 种结果都有可能。

## 11.3
关于 Cache 一致性协议，MESI 协议比 ESI 协议增加了 M 状态，请解释有什么好处。

**解**
M 即 Modified，表示该 Cache 行被当前处理器核独占且修改了。同时，E（Exclusive，独占）状态表示该 Cache 行被当前处理器独占且没有修改。

可以看出，MESI 协议中的 M 状态是 ESI 协议中 E 状态的子集，使得没有被修改的 E 状态 Cache 行在替换时不再需要写回内存，减少了 Cache 到内存的数据传输次数。

## 11.4
请分别采用 `Fetch_and_Increment` 和 `Compare_and_Swap` 原子指令编写实现自旋锁的代码，并分析可能的性能改进措施。

**解**
使用 `Fetch_and_Increment`：
```c
typedef struct spinlock {
    int status;
} spinlock_t;

void acquire_spinlock1(spinlock * lock) {
    while (Fetch_and_Increment(lock->status));
}

void release_spinlock1(spinlock * lock) {
    lock->status = 0;
}

void do_something() {
    spinlock_t lock;
    acquire_spinlock1(&lock);
    // critical_section
    release_spinlock1(&lock);
}
```

使用 `Compare_and_Swap`：
```c
typedef struct spinlock {
    int status;
} spinlock_t;

void acquire_spinlock2(spinlock * lock) {
    int not_success = 1;
    while (not_success) {
        Compare_and_Swap(&lock->status, 0, &not_success);
    }
}

void release_spinlock2(spinlock * lock) {
    lock->status = 0;
}

void do_something() {
    spinlock_t lock;
    acquire_spinlock2(&lock);
    // critical_section
    release_spinlock2(&lock);
}
```

自旋锁对锁变量会出现访存冲突，一个核获得锁后，其他处理器会不断的访问锁变量，形成大量的访存通信。可以在自旋过程中添加延迟以减轻访存压力，也可以通过实现互斥锁等锁结构来优化。

## 11.5
在共享存储的多处理器中，经常会出现假共享现象。假共享是由于两个变量处于同一个 Cache 行中引起的，会对性能造成损失。为了尽量减少假共享的发生，程序员在写程序时应该注意什么？

**解**

* 在编写程序时，避免使多个处理器、进程、线程同时访问全局或动态分配的相邻数据结构，例如同时访问一个全局结构体的变量 A 和 B；
* 使用编译指令强制对齐单个变量、填充数据结构使之与 Cache 行对齐，从而避免引发假共享的两个变量处于同一个 Cache 行；
* 将全局数据复制到本地副本后再进行操作，以避免对全局数据的频繁访问；
* 测试时利用 CPU 厂商提供的实用程序进行监测。
## 11.6
请介绍片上网络路由器设计中的虚通道概念，并说明采用虚通道有什么好处。

**解**

虚通道是分时复用物理通道的一种表示，输入单元包含能缓存数个 flit 及其状态信息的缓存，相当于多个数据传输通路。

使用虚通道能够分时复用物理通道，节约了片上资源。

## 11.7
分析 Fermi GPU 的存储结构，指出不同层次存储结构的带宽、延迟，以及是否共享。

**解**

|存储结构|位置|容量|带宽|延迟|共享|备注|
|:-:|:-:|:-:|:-:|:-:|:-:|:-|
|SM 寄存器堆|SM 内部|128 KB|8000 GB/s|1 cycle|否
|共享存储|SM 内部|16 KB / 48 KB|1000 GB/s|20-30 cycles|是|与 L1 Cache 总计 64 KB 每 SM|
|L1 Cache |SM 内部|16 KB / 48 KB|1000 GB/s|20-30 cycles|否|与共享存储总计 64 KB 每 SM|
|L2 Cache |GPU|768 KB|-|-|是|未找到带宽、延迟的数据|
|主存储|DRAM|最大 6GB|177 GB/s| 400-800 cycles|是|GDDR5，与 CPU 共享|

<!-- * 每个 SM（流式多处理器，Streaming Multiprocessors）中，包含一个巨大的寄存器堆，大小为 128 KB。每个 SM 中又包含 32 个核心（Core）。
* 每个 SM 中，还包含 L1 缓存以及 SMEM（共享内存，Shared Memory）。它们的大小总共为 64 KB（48 KB Shared / 16 KB L1，或者 16 KB Shared / 48 KB L1）。
* 所有 SM 共用 L2 缓存，大小为 768 KB。
* CPU 和 GPU 同时可访问主存储体，大小最大为 6 GB。 -->

# 计算机体系结构基础 - 第 12 章作业

## 12.1
写两个简单的测试程序，用于测量一台计算机系统最大的 MIPS 和最大的 MFLOPS 的值。

**解**

1. 用于测量 MIPS 的 C 语言程序：（计算斐波那契数列）
   ```c
   #include <stdio.h>
   #define FIB_MAX 100000000
   #define LOOP_MAX 100

   int T = LOOP_MAX;
   long a, b, c;

   int main() {
       while (T--) {
           a = 0;
           b = 1;
           for (int i = 0; i < FIB_MAX; i++) {
               c = a + b;
               a = b;
               b = c;
           }
       }
       return 0;
   }
   ```

   其运行结果为
   ```shell
   $ gcc -O0 Fibonacci.c
   $ time ./a.out

   real    0m27.919s
   user    0m27.919s
   sys     0m0.000s
   ```

   通过 `objdump` 工具进行反汇编，得到其反汇编代码：
   ```
   0000000000001129 <main>:
       1129:	f3 0f 1e fa          	endbr64 
       112d:	55                   	push   %rbp
       112e:	48 89 e5             	mov    %rsp,%rbp
       1131:	eb 60                	jmp    1193 <main+0x6a>
       1133:	48 c7 05 f2 2e 00 00 	movq   $0x0,0x2ef2(%rip)        # 4030 <a>
       113a:	00 00 00 00 
       113e:	48 c7 05 d7 2e 00 00 	movq   $0x1,0x2ed7(%rip)        # 4020 <b>
       1145:	01 00 00 00 
       1149:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%rbp)
       1150:	eb 38                	jmp    118a <main+0x61>
       1152:	48 8b 15 d7 2e 00 00 	mov    0x2ed7(%rip),%rdx        # 4030 <a>
       1159:	48 8b 05 c0 2e 00 00 	mov    0x2ec0(%rip),%rax        # 4020 <b>
       1160:	48 01 d0             	add    %rdx,%rax
       1163:	48 89 05 be 2e 00 00 	mov    %rax,0x2ebe(%rip)        # 4028 <c>
       116a:	48 8b 05 af 2e 00 00 	mov    0x2eaf(%rip),%rax        # 4020 <b>
       1171:	48 89 05 b8 2e 00 00 	mov    %rax,0x2eb8(%rip)        # 4030 <a>
       1178:	48 8b 05 a9 2e 00 00 	mov    0x2ea9(%rip),%rax        # 4028 <c>
       117f:	48 89 05 9a 2e 00 00 	mov    %rax,0x2e9a(%rip)        # 4020 <b>
       1186:	83 45 fc 01          	addl   $0x1,-0x4(%rbp)
       118a:	81 7d fc ff e0 f5 05 	cmpl   $0x5f5e0ff,-0x4(%rbp)
       1191:	7e bf                	jle    1152 <main+0x29>
       1193:	8b 05 77 2e 00 00    	mov    0x2e77(%rip),%eax        # 4010 <T>
       1199:	8d 50 ff             	lea    -0x1(%rax),%edx
       119c:	89 15 6e 2e 00 00    	mov    %edx,0x2e6e(%rip)        # 4010 <T>
       11a2:	85 c0                	test   %eax,%eax
       11a4:	75 8d                	jne    1133 <main+0xa>
       11a6:	b8 00 00 00 00       	mov    $0x0,%eax
       11ab:	5d                   	pop    %rbp
       11ac:	c3                   	retq   
       11ad:	0f 1f 00             	nopl   (%rax)
   ```

   分析反汇编代码，估算出其执行的代码条数为 5 + 100 × (4 + 2 + 10<sup>8</sup> × (9 + 2) + 5) = 1.1 × 10<sup>11</sup>

   由此计算得到 MIPS 约为 3940。通过 7-Zip 的基准测试得到的 MIPS 值约为 3200，差距在可接受范围内。

2. 用于测量 MFLOPS 的 C 语言程序：（牛顿迭代法求 2 的平方根）
   ```c
   #include <stdio.h>
   #define NEW_MAX 100000000
   #define LOOP_MAX 50

   int T = LOOP_MAX;
   double ans;
   int main() {
       while (T--) {
           ans = 4.0;
           for (int i = 0; i < NEW_MAX; i++) {
               ans = (ans + 2.0 / ans) * 0.5;
           }
       }
       return 0;
   }
   ```

   其运行结果为
   ```shell
   $ gcc -O0 Newton.c 
   $ time ./a.out

   real    0m42.531s
   user    0m42.530s
   sys     0m0.000s
   ```

   分析程序，估算出其进行的浮点操作数为 1.5 × 10<sup>10</sup>。

   由此计算得到 MFLOPS 约为 352.7。


## 12.2
阅读和分析 STREAM v1 基准测试程序：
1. 测出一台计算机上的测试结果并给出分析报告。
2. 调节处理器的频率，看内存的带宽和频率的关系。
3. 修改 STREAM 测试程序，看单精度带宽和双精度带宽的差别。

**解**
1. 测试结果如下：
   ```shell
   $ gcc -O stream.c -o stream
   $ ./stream 
   -------------------------------------------------------------
   STREAM version $Revision: 5.10 $
   -------------------------------------------------------------
   This system uses 8 bytes per array element.
   -------------------------------------------------------------
   Array size = 10000000 (elements), Offset = 0 (elements)
   Memory per array = 76.3 MiB (= 0.1 GiB).
   Total memory required = 228.9 MiB (= 0.2 GiB).
   Each kernel will be executed 10 times.
    The *best* time for each kernel (excluding the first iteration)
    will be used to compute the reported bandwidth.
   -------------------------------------------------------------
   Your clock granularity/precision appears to be 1 microseconds.
   Each test below will take on the order of 9442 microseconds.
      (= 9442 clock ticks)
   Increase the size of the arrays if this shows that
   you are not getting at least 20 clock ticks per test.
   -------------------------------------------------------------
   WARNING -- The above is only a rough guideline.
   For best results, please be sure you know the
   precision of your system timer.
   -------------------------------------------------------------
   Function    Best Rate MB/s  Avg time     Min time     Max time
   Copy:           15112.9     0.011500     0.010587     0.015825
   Scale:          13976.4     0.012203     0.011448     0.013891
   Add:            16304.4     0.015601     0.014720     0.017893
   Triad:          16248.1     0.015614     0.014771     0.017890
   -------------------------------------------------------------
   Solution Validates: avg error less than 1.000000e-13 on all three arrays
   -------------------------------------------------------------
   ```

   观察测试结果，可以发现 Add、Triad 操作的带宽最大，Copy 次之，Scale 最小。分析这四种操作的源代码：
   ```c
       c[j] = a[j];                 // Copy
       b[j] = scalar*c[j];          // Scale
       c[j] = a[j]+b[j];            // Add
       a[j] = b[j]+scalar*c[j];     // Triad
   ```
   可以看出，单次 Copy、Scale 操作仅需 2 次访存，而单次 Add、Triad 操作需要3次访存。在单次操作内，访存次数越多，单次访存所均摊的其他操作（计算）时间越少，访存带宽越大，故而 Add、Triad 操作带宽大于 Copy 和 Scale；在单次操作内，其他操作（计算）越复杂，单次访存所均摊的其他操作时间越多，访存带宽越小，故而 Copy 带宽大于 Scale，Add 带宽大于 Triad。

2. 通过调整电源设置来调控 CPU 频率，依次进行测试，得到结果如下表：
   <table>
    <thead>
     <tr>
      <th rowspan="2" style="text-align: center;">电源设置</th>
      <th rowspan="2" style="text-align: center;">CPU 频率</th>
      <th colspan="4" style="text-align: center;">Best Rate MB/s</th>
     </tr>
     <tr>
      <th style="text-align: center;">Copy</th>
      <th style="text-align: center;">Scale</th>
      <th style="text-align: center;">Add</th>
      <th style="text-align: center;">Triad</th>
     </tr>
    </thead>
    <tbody>
     <tr>
      <td style="text-align: center;">最佳性能</td>
      <td style="text-align: center;">3.5 GHz</td>
      <td style="text-align: center;">15112.9</td>
      <td style="text-align: center;">13976.4</td>
      <td style="text-align: center;">16304.4</td>
      <td style="text-align: center;">16248.1</td>
     </tr>
     <tr>
      <td style="text-align: center;">更好的性能</td>
      <td style="text-align: center;">2.3 GHz</td>
      <td style="text-align: center;">11768.3</td>
      <td style="text-align: center;">11382.9</td>
      <td style="text-align: center;">12617.6</td>
      <td style="text-align: center;">12583.9</td>
     </tr>
     <tr>
      <td style="text-align: center;">节电模式</td>
      <td style="text-align: center;">1.0 GHz</td>
      <td style="text-align: center;">9193.4</td>
      <td style="text-align: center;">8153.7</td>
      <td style="text-align: center;">8998.2</td>
      <td style="text-align: center;">9797.5</td>
     </tr>
    </tbody>
   </table>

   观察数据可以发现，CPU 频率与内存带宽正相关。

3. 将代码中的宏定义 `#define STREAM_TYPE float` 修改为 `#define STREAM_TYPE float`，重新编译测试，得到如下结果：
   ```shell
   $ gcc -O stream-float.c -o stream-float
   $ ./stream-float 
   -------------------------------------------------------------
   STREAM version $Revision: 5.10 $
   -------------------------------------------------------------
   This system uses 4 bytes per array element.
   -------------------------------------------------------------
   Array size = 10000000 (elements), Offset = 0 (elements)
   Memory per array = 38.1 MiB (= 0.0 GiB).
   Total memory required = 114.4 MiB (= 0.1 GiB).
   Each kernel will be executed 10 times.
    The *best* time for each kernel (excluding the first iteration)
    will be used to compute the reported bandwidth.
   -------------------------------------------------------------
   Your clock granularity/precision appears to be 1 microseconds.
   Each test below will take on the order of 4943 microseconds.
      (= 4943 clock ticks)
   Increase the size of the arrays if this shows that
   you are not getting at least 20 clock ticks per test.
   -------------------------------------------------------------
   WARNING -- The above is only a rough guideline.
   For best results, please be sure you know the
   precision of your system timer.
   -------------------------------------------------------------
   Function    Best Rate MB/s  Avg time     Min time     Max time
   Copy:           14492.5     0.005902     0.005520     0.007029
   Scale:          11889.0     0.006895     0.006729     0.007270
   Add:            15394.3     0.007995     0.007795     0.008764
   Triad:          15308.1     0.008098     0.007839     0.008688
   -------------------------------------------------------------
   Solution Validates: avg error less than 1.000000e-06 on all three arrays
   -------------------------------------------------------------
   ```
   观察数据可以发现，其访存带宽略小于采用双精度浮点数。

   进一步测试发现，当关闭所有编译优化选项之后，单精度带宽约是双精度带宽的一半左右。


> 参考文档：STREAM Benchmark Reference Information - http://www.cs.virginia.edu/stream/ref.html

## 12.3
分析 SPEC CPU2006 中 `462.libquantum` 程序，看它对处理器微结构的压力在哪里。查阅 `spec.org` 网站，看不同编译器对 `462.libquantum` 的分值的影响，猜测 Intel 编译器 `icc` 采用了什么编译技术使得其分值能达到上百分。

**解**

`libquantum` 模拟量子计算机的 C 语言库，其需要模拟大量量子比特的状态变化，这些过程在量子计算机上是并行的，但在 `libquantum` 中需要串行完成。

查看 `spec.org` 网站上的测试结果，使用 `icc` 的测试得分要比其他的高得多。根据 `462.libquantum` 的特点，`icc` 可能采用了将串行程序转换为并行的技术，具有高并行、高向量化的特点。

## 12.4
使用 `perf` 工具，测量各种排序算法如冒泡排序、希尔排序等算法的 IPC，分析排序算法对处理器的微结构的压力在哪里。

**解**

* 冒泡排序的 C 语言程序：
  ```c
  #include <stdio.h>
  #define LENGTH 10
  int arr[LENGTH] = {10, 8, 6, 4, 2, 9, 7, 5, 3, 1};

  void bubble_sort(int * arr, int len) {
      int tmp;
      for (int i = 0; i < len - 1; i++) {
          for (int j = 0; j < len - i - 1; j++) {
              if (arr[j] > arr[j + 1]) {
                  tmp = arr[j];
                  arr[j] = arr[j + 1];
                  arr[j + 1] = tmp;
              }
          }
      }
  }

  int main() {
      for (int i = 0; i < LENGTH; i++) {
          printf("%d ", arr[i]);
      }
      printf("\n");

      bubble_sort(arr, LENGTH);

      for (int i = 0; i < LENGTH; i++) {
          printf("%d ", arr[i]);
      }
      printf("\n");

      return 0;
  }
  ```

  通过 `perf` 分析的结果如下：
  ```shell
  $ perf stat ./a.out 
  10 8 6 4 2 9 7 5 3 1 
  1 2 3 4 5 6 7 8 9 10 

   Performance counter stats for './a.out':

                0.48 msec task-clock:u              #    0.615 CPUs utilized
                   0      context-switches:u        #    0.000 K/sec
                   0      cpu-migrations:u          #    0.000 K/sec
                  49      page-faults:u             #    0.000 K/sec
     <not supported>      cycles:u
     <not supported>      instructions:u
     <not supported>      branches:u
     <not supported>      branch-misses:u

         0.000776300 seconds time elapsed

         0.000850000 seconds user
         0.000000000 seconds sys
  ```


* 希尔排序的 C 语言程序：
  ```c
  #include <stdio.h>
  #define LENGTH 10
  int arr[LENGTH] = {10, 8, 6, 4, 2, 9, 7, 5, 3, 1};

  void shell_sort(int * arr, int len) {
      int tmp;

      for (int g = len / 2; g > 0; g /= 2) {
          for (int i = g; i < len ; i++) {
              int temp = arr[i];
              int j = i - g;
              while (j >= 0 && arr[j] > temp) {
                  arr[j + g] = arr[j];
                  j -= g;
              }
              arr[j + g] = temp;
          }
      }
  }

  int main() {
      for (int i = 0; i < LENGTH; i++) {
          printf("%d ", arr[i]);
      }
      printf("\n");

      shell_sort(arr, LENGTH);

      for (int i = 0; i < LENGTH; i++) {
          printf("%d ", arr[i]);
      }
      printf("\n");

      return 0;
  }
  ```

  通过 `perf` 分析的结果如下：
  ```shell
  $ perf stat ./a.out 
  10 8 6 4 2 9 7 5 3 1 
  1 2 3 4 5 6 7 8 9 10 

   Performance counter stats for './a.out':

                0.32 msec task-clock:u              #    0.619 CPUs utilized
                   0      context-switches:u        #    0.000 K/sec
                   0      cpu-migrations:u          #    0.000 K/sec
                  51      page-faults:u             #    0.000 K/sec
     <not supported>      cycles:u
     <not supported>      instructions:u
     <not supported>      branches:u
     <not supported>      branch-misses:u

         0.000515300 seconds time elapsed

         0.000587000 seconds user
         0.000000000 seconds sys
  ```
  
由于我没有安装在实体机器上的 Linux 操作系统，我只能在虚拟机（WSL2）中进行测试。但 WSL2 目前还不支持硬件性能计数器，导致无法获取到实际的 IPC。  

## 12.5
使用 `gprof` 工具，获得 `linpack` 程序的热点函数。

**解**
经过几轮尝试，没能成功安装 `linpack`，被卡在编译 GotoBLAS 的步骤……

参照其他同学的结果，`linpack` 中的热点函数是 `daxpy`。

## 12.6
使用 LMbench 测试程序，获得 CPU 的一级、二级、三级 Cache 和内存的访存延迟。

**解**

在运行 LMbench 测试程序的过程中，程序卡死在“Calculating memory load latency”一步……

## 12.7
使用 SimpleScalar 模拟器，分析二级 Cache 的延迟对性能的影响（从 24 变到 12 个时钟周期）假设使用 Alpha 指令集，测试程序为 SPEC CPU2000 的 `164.bzip` 和 `253.perlbmk`。

**解**

SimpleScalar 模拟器比较古老，其标准安装环境是 2010 年发布的 Ubuntu 10.04。虽然有人尝试在 Ubuntu 14.04 或更高的版本上安装，但都遇到了很多依赖不兼容的问题。因此我选择使用现成的 Docker 环境（https://hub.docker.com/r/khaledhassan/simplescalar）。

但在完成环境配置之后，研究了很长时间的 SimplerScalar 和 SPEC CPU2000 的文档，我还是没有搞明白如何让 SPEC CPU2000 在 SimpleScalar 上编译运行……

## 12.8
嵌入式基准测试程序如 EEMBC 和桌面基准测试程序在行为特性上有什么差别？

**解**

对于嵌入式系统，其 Soc 的微结构要比一般的桌面 CPU 简单很多。Cache、乱序执行、重命名寄存器等桌面 CPU 常见的技术很少会在资源紧张的嵌入式系统上应用。因此，嵌入式基准测试程序不会关注这些方面。

相比较桌面系统，嵌入式系统对功耗要求更高，嵌入式基准测试程序可能会专门对功耗进行测试。

## 12.9
查找 ARM Cortex A 系列处理器的用户手册，列出你认为比较重要的硬件性能计数器的 10 个性能事件，并给出事件描述。

**解**
|事件名称|事件描述|
|:-|:-|
|Instruction cache dependent stall cycles|统计处理器等待指令到达的周期数。处理器在此时准备好接受指令，但指令缓存正在进行一次行填充而不能提供指令。|
|Data cache dependent stall cycles|统计处理器等待数据到达的周期数。处理器在此时准备好接受数据，但指令缓存正在进行一次行填充而不能提供数据。|
|Main TLB miss stall cycles|统计处理器等待主 TLB 完成遍历的周期数|
|Main execution unit instructions|统计主处理单元（主执行流水线、乘法流水线、ALU 流水线）中正在执行的指令数|
|Second execution unit instructions|统计次处理单元中正在执行的指令数|
|Load/Store Instructions|统计访存指令条数|
|Floating-point instructions|统计浮点指令条数|
|External interrupts|统计外部中断的次数|
|Processor stalled because of a write to memory|统计因写内存导致的处理器暂停周期数|
|Processor stalls because of PLDs|统计由于 PLD 满导致的处理器暂停周期数|


## 12.10
模拟建模的方法和性能测量的方法相比有哪些优点？

**解**

* 性能测量的方法仅能用于已经搭建好的系统和原型系统，但模拟建模的方法可以用于系统的设计阶段；
* 使用性能测量的方法，必须线搭建好原型系统，开销较大，但模拟建模的方法只需要通过软件建模，开销较小；
* 性能测量的方法依赖于系统中的硬件性能计数器，灵活度低，模拟建模的方法可以方便地增添性能事件，更灵活的进行统计测量。

## 12.11
SimPoint 的基本原理是什么，为什么其能减少模拟建模的时间？

**解**

SimPoint 是一种采样模拟技术，它找到程序执行的相位，然后对能够代表每个相位的部分进行采样和模拟仿真。其避免了模拟时的大量重复，以相同相位作代表，从而大量的节省了时间。

## 12.12
模拟器和真实的机器怎么校准，校准的评价指标通常是什么？

**解**

比较模拟器和真实机器在同一程序上的运行结果，对比相关性能计数器的数据。若发现某些数据存在较大的误差，则寻找出问题所在予以修正。

评价指标即为模拟器与真实机器运行结果的吻合度。

## 12.13
在你的电脑上运行 SPEC CPU2000 的 rate 并给出分值。

**解**

|SPEC 程序|`-O0` 运行时间 / 秒|`-O0` 分值|`-O2` 运行时间 / 秒|`-O2` 分值|
|:-|:-:|:-:|:-:|:-:|
|164.gzip|76.5|1830|121|1161|
|175.vpr|55.3|2530|101|1380|
|176.gcc|27.9|3938|X|X|
|181.mcf|72.4|2488|90.1|1997|
|186.crafty|28.4|3516|41.1|2430|
|197.parser|93|1935|155|1160|
|252.eon|X|X|232|560|
|253.perlbmk|X|X|72.1|2496|
|254.gap|31.6|3476|31.1|3538|
|255.vortex|46.6|4079|85.3|2227|
|256.bzip2|60.3|2486|129|1166|
|300.twolf|83.6|3587|150|1998|
|SPEC_INT2000||2878 ||1640 |
|168.wupwise|X|X|104|1535|
|171.swim|X|X|164|1886|
|172.mgrid|X|X|334|539|
|173.applu|X|X|286|734|
|177.mesa|33.8|4143|68.3|2051|
|178.galgel|X|X|X|X|
|179.art|23.3|11148|51.2|5078|
|183.equake|X|X|X|X|
|187.facerec|36.7|5176|76.2|2493|
|188.ammp|X|X|173|1273|
|189.lucas|X|X|69|2899|
|191.fma3d|X|X|124|1687|
|200.sixtrack|X|X|257|429|
|301.apsi|54.8|4746|200|1297|
|SPEC_INT2000||5804 ||1471 |

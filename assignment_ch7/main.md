# 计算机体系结构基础 - 第 7 章作业

## 7.1
什么情况下需要对 Cache 进行初始化？Cache 初始化过程中所使用的 Cache 指令 Index Store Tag（如 7.1.4 节的示例代码中所示）的作用是什么？列举一种 MIPS 的其他 Cache 指令并解释。

**解**

在系统复位后 Cache 处于未经初始化的状态，如果使用 Cache 空间可能导致错误命中，此时需要对 Cache 进行初始化。

Cache 指令 Index Store Tag 的作用是：使用地址索引到 Cache 行，根据 CP0 寄存器 LagLo 指定的 Tag、V、D 域更新该 Cache 行。

在 Cache 初始化过程中，我们首先初始化 CP0 寄存器 TagLo 为 0，然后再利用 Cache 指令 Index Store Tag 给所有 Cache 行写空 TAG。

其他的 MIPS Cache 指令如 Cache 指令 Index Invalid，它的作用是：使用地址索引到 Cache 行，然后使该行无效。

## 7.2
Cache 初始化和内存初始化的目的有什么不同？系统如何识别内存的更换？

**解**

Cache 初始化的目的，是为了清除 Cache 中残留或随机的数据，防止访问 Cache 空间时导致错误的命中，涉及的是 Cache 内部存储的初始数据；内存初始化的目的，则是根据内存的配置信息正确初始化，以便后续程序能够正确地访问内存，并不涉及存储的初始数据。

内存的更换应当是在系统关闭的情况下进行的。在系统复位后的初始化过程中，BIOS 会自动通过 I2C 总线读取内存条上的 SPD 芯片，从中加载内存配置信息，并根据这些信息配置 CPU 或桥片中的内存控制器。如果系统发现此次读取的 SPD 信息与上一次初始化时读取的不相同，这说明内存发生了更换。

## 7.3
从 HyperTransport 配置地址空间的划分上，计算整个系统能够支持的总线数量、设备数量及功能数量。

**解**

在 HyperTransport 总线中：
* 总共支持的总线数量为 $256$：配置访问中总线号为第 $16 \sim 23$ 位，共 $8$ 位，可取 $2^8 = 256$ 种值；
* 单条总线支持的设备数量为 $32$：配置访问中设备号为第 $15 \sim 11$ 位，共 $5$ 位，可取 $2^5 = 32$ 种值；
* 单个设备支持的功能数量为 $8$：配置访问中功能号为第 $10 \sim 8$ 位，共 $3$ 位，可取 $2^3 = 8$ 种值；

## 7.4
根据 PCI 地址空间命中方法及 BAR 的配置方式，给出地址空间命中公式。

**解**

IO、Memory 空间命中公式为：`(ADDR & MASK) == {BAR[31:4], 4'b0000}`；

其中 `MASK` 由 BAR 空间的大小决定，可由下面的方法得到：
* 向 BAR 寄存器写入 `32'h_ffffffff`；
* 读取 BAR 的值到 `BAR_test`；
* 取 `{BAR_test[31:4], 4'b0000}` 为 `MASK`。

配置空间命中公式为：`(ADDR & 32'h_ff000000) == {PCI_Config[31:24], 24'h000000}`；

## 7.5
多核唤醒时，如果采用核间中断方式，从核的唤醒流程是怎样的？

**解**

唤醒流程如下：
1. 主核准备好唤醒程序的目标地址、执行参数等数据；
2. 主核触发从核的核间中断，并挂起主核；
3. 从核处理中断，跳转到唤醒程序开始执行唤醒过程；
4. 从核完成唤醒，触发主核的核间中断；
5. 主核处理中断，继续尝试唤醒下一个从核。

## 7.6
在一台 Linux 机器上，通过“`lspci -v`”指令查看该机器的设备列表，并列举其中三个设备的总线号、设备号和功能号，通过其地址空间信息写出该设备 BAR 的实际内容。

**解**

运行 `lspci -v` 指令的结果如下（仅列举其中的三个设备）：
```shell
$ lspci -v
# ...
00:01.2 USB controller: Intel Corporation 82371SB PIIX3 USB [Natoma/Triton II] (rev 01)
(prog-if 00 [UHCI])
        Subsystem: Red Hat, Inc. QEMU Virtual Machine
        Flags: bus master, fast devsel, latency 0, IRQ 11
        I/O ports at c040 [size=32]
        Kernel driver in use: uhci_hcd

00:02.0 VGA compatible controller: Cirrus Logic GD 5446 (prog-if 00 [VGA controller])
        Subsystem: Red Hat, Inc. QEMU Virtual Machine
        Flags: fast devsel
        Memory at fc000000 (32-bit, prefetchable) [size=32M]
        Memory at febd0000 (32-bit, non-prefetchable) [size=4K]
        Expansion ROM at 000c0000 [disabled] [size=128K]
        Kernel driver in use: cirrus
        Kernel modules: cirrusfb, cirrus
        
00:05.0 SCSI storage controller: Red Hat, Inc. Virtio block device
        Subsystem: Red Hat, Inc. Virtio block device
        Physical Slot: 5
        Flags: bus master, fast devsel, latency 0, IRQ 10
        I/O ports at c000 [size=64]
        Memory at febd3000 (32-bit, non-prefetchable) [size=4K]
        Capabilities: [40] MSI-X: Enable+ Count=2 Masked-
        Kernel driver in use: virtio-pci
# ...
```

这三个设备的总线号、设备号、功能号、BAR 如下表：

<table>
<thead>
<tr>
<th style="text-align:left">总线号</th>
<th style="text-align:left">设备号</th>
<th style="text-align:left">功能号</th>
<th style="text-align:left">设备名</th>
<th style="text-align:left">BAR</th>
<th style="text-align:left">可写位</th>
<th style="text-align:left">只读 0 位</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align:left"><code>00</code></td>
<td style="text-align:left"><code>01</code></td>
<td style="text-align:left"><code>2</code></td>
<td style="text-align:left">USB controller</td>
<td style="text-align:left"><code>32'h_0000c041</code></td>
<td style="text-align:left"><code>[31: 5]</code></td>
<td style="text-align:left"><code>[ 4: 4]</code></td>
</tr>
<tr>
<td style="text-align:left" rowspan="3"><code>00</code></td>
<td style="text-align:left" rowspan="3"><code>02</code></td>
<td style="text-align:left" rowspan="3"><code>0</code></td>
<td style="text-align:left" rowspan="3">VGA compatible controller</td>
<td style="text-align:left"><code>32'h_fc000000</code></td>
<td style="text-align:left"><code>[31:25]</code></td>
<td style="text-align:left"><code>[24: 4]</code></td>
</tr>
<tr>
<td style="text-align:left"><code>32'h_febd0000</code></td>
<td style="text-align:left"><code>[31:12]</code></td>
<td style="text-align:left"><code>[11: 4]</code></td>
</tr>
<tr>
<td style="text-align:left"><code>32'h_000c0000</code></td>
<td style="text-align:left"><code>[31:17]</code></td>
<td style="text-align:left"><code>[16: 4]</code></td>
</tr>
<tr>
<td style="text-align:left" rowspan="2"><code>00</code></td>
<td style="text-align:left" rowspan="2"><code>05</code></td>
<td style="text-align:left" rowspan="2"><code>0</code></td>
<td style="text-align:left" rowspan="2">SCSI storage controller</td>
<td style="text-align:left"><code>32'h_0000c001</code></td>
<td style="text-align:left"><code>[31: 6]</code></td>
<td style="text-align:left"><code>[ 5: 4]</code></td>
</tr>
<tr>
<td style="text-align:left"><code>32'h_febd3000</code></td>
<td style="text-align:left"><code>[31:12]</code></td>
<td style="text-align:left"><code>[11: 4]</code></td>
</tr>
</tbody>
</table>
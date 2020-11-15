# 计算机体系结构基础 - 第 3 章作业

## 3.1
简述 MIPS 与 X86 在异常处理过程中的区别。

**解**

1. 异常处理准备：
   * MIPS 中，被异常打断的指令地址存放在协处理器 CP0 的 EPC 寄存器中；
   * X86 中，被异常打断的指令地址（CS、EIP 组合）被存放在栈中；
2. 确定异常来源：
   * MIPS 中，区分异常来源的工作由软件完成，异常处理程序根据 CAUSE 寄存器存储的状态，再做进一步查询和区分处理；
   * X86 中，区分异常来源的工作由硬件完成，硬件根据异常生成异常中断号，根据编号查找预设的中断描述符表，跳转到对应异常处理程序入口；
3. 保存执行状态、处理异常、恢复执行状态，这两部分 MIPS 与 X86 没有太大区别；
4. 异常返回：
   * MIPS 中，使用 ERET 指令从异常中返回；X86 中，使用 IRET 指令从异常中返回。两者功能相差不大。

## 3.2
MIPS 的 LL bit 在发生异常后会怎样处理，为什么？

**解**

在发生异常后，LL bit 会被重置，使得后面的 SC 指令执行失败。

LL bit 用于标识 LL、SC 指令对在执行时的原子性。在 LL、SC 指令在运用过程中发生异常，异常本身会打破 LL、SC 指令的原子性。原子性的丢失意味着 LL bit 必须被重置，需要重新尝试 LL、SC 指令来保证原子性。

## 3.3
简述精确异常与非精确异常的区别。找一个使用非精确异常的处理器。

**解**

精确异常（Precise Exception），要求被异常打断的指令前的指令都执行完毕，被异常打断前的指令及以后的所有指令都如同没执行。在精确异常中，EPTR 必须恰好指向被异常打断的指令。

非精确异常（Imprecise Exception），对被异常打断前后的指令执行情况没有要求。在非精确异常中，EPTR 不一定恰好指向被异常打断的指令，通常指向被异常打断的指令后的某条指令。

在 SONY、TOSHIBA、IBM 开发的 Cell 处理器架构中，浮点指令会产生非精确异常。

## 3.4
在一台 MIPS-Linux 机器（页大小为 4KB）上执行下列程序片段，会发生多少次异常？说明其过程。

```c
void cycle(double *a) {
    int i;
    double b[65536];
    for (i = 0; i < 3 ; i++) {
        memcpy(a, b, sizeof(b));
    }
}
```

**解**

假设 TLB 表能存储超过 256 项。

单个 `double` 变量为 8 个字节，长度为 `65536` 的数组 `b` 大小即为 `65536 × 8 B = 512 KB`，共 `128` 页。

第 1 次 `memcpy` 的过程中：
* 访问 `a`：由于 TLB 内没有 `a` 所在内存的相应表项，会造成 `128` 次 TLB 充填异常。如果 `a` 还没有分配物理空间，则还会继续造成 `128` 次 TLB 无效异常。
* 访问 `b`：由于 TLB 内没有 `b` 所在内存的相应表项，会造成 `128` 次 TLB 充填异常。同时，`b` 还没有分配物理空间，还会造成 `128` 次 TLB 无效异常。

在后两次 `memcpy` 的过程中，由于访问内存所需的 TLB 已经全部填充完成，不再产生异常。

故总共产生 512 次异常。

## 3.5 
用 C 语言描述包含 TLB 的页式存储管理过程（包含 TLB 操作）。

**解**

程序代码如下：
```c
typedef struct {
    asid_t ASID;
    vpn_t VPN; 
    mask_t Mask;
    pfn_t PFN;
    flag_t Flag;
} TLB_page_table_entry_t;

TLB_page_table_entry_t TLB_table[TLB_SIZE];

pfn_t address_translation(asid_t ASID, vpn_t VPN) {
    search_result_t search_result;

    if (!(search_result = TLB_probe(ASID, VPN))) {
        raise_exception(EXCEPTION_TLB_REFILL);
    }
    if (!search_result.V) {
        raise_exception(EXCEPTION_TLB_INVALID);
    }
    if (!search_result.D) {
        raise_exception(EXCEPTION_TLB_MODIFY);
    }
    return search_result.PFN;
}

void handle_TLB_refill(void) {
    context_t context = get_cp0(CP0_CONTEXT);
    pfn_t PFN = get_PFN_from_page_table(context);
    TLB_write(context, PFN);
    return;
}

void handle_TLB_invalid(void) {
    pte_t pte = get_pte_current();
    read_page_from_extern();
}

void handle_TLB_modify(void) {
    pte_t pte = get_pte_current();
    if (check_something()) {
        enable_write();
    } else {
        kill();
    }
}
```
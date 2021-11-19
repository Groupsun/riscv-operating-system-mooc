# 练习

## 练习4-1

编写⼀个简单的打印 “hello world!” 的程序源文件：`hello.c`如下所示：

```c
#include <stdio.h>

int main(){
        printf("hello world!\n");
        return 0;
}
```

对源文件进⾏编译，⽣成针对⽀持rv32ima指令集架构处理器的⽬标文件`hello.o`：

```bash
# 这里我使用了gnu动态编译，不知道为何使用elf会出错：
riscv64-unknown-linux-gnu-gcc -march=rv32ima -mabi=ilp32 hello.c -o hello.o
```

查看`hello.o`的文件的文件头信息：

```bash
$ riscv64-unknown-linux-gnu-readelf -h hello.o
ELF Header:
  Magic:   7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF32
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           RISC-V
  Version:                           0x1
  Entry point address:               0x102f0
  Start of program headers:          52 (bytes into file)
  Start of section headers:          11064 (bytes into file)
  Flags:                             0x1, RVC, soft-float ABI
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         9
  Size of section headers:           40 (bytes)
  Number of section headers:         34
  Section header string table index: 33
```

查看`hello.o`的Section header table：

```bash
$ riscv64-unknown-linux-gnu-readelf -SW hello.o
There are 34 section headers, starting at offset 0x2b38:

Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .interp           PROGBITS        00010154 000154 000021 00   A  0   0  1
  [ 2] .note.ABI-tag     NOTE            00010178 000178 000020 00   A  0   0  4
  [ 3] .hash             HASH            00010198 000198 000024 04   A  5   0  4
  [ 4] .gnu.hash         GNU_HASH        000101bc 0001bc 00002c 04   A  5   0  4
  [ 5] .dynsym           DYNSYM          000101e8 0001e8 000040 10   A  6   1  4
  [ 6] .dynstr           STRTAB          00010228 000228 00003f 00   A  0   0  1
  [ 7] .gnu.version      VERSYM          00010268 000268 000008 02   A  5   0  2
  [ 8] .gnu.version_r    VERNEED         00010270 000270 000020 00   A  6   1  4
  [ 9] .rela.plt         RELA            00010290 000290 000018 0c  AI  5  19  4
  [10] .plt              PROGBITS        000102b0 0002b0 000040 10  AX  0   0 16
  [11] .text             PROGBITS        000102f0 0002f0 00012a 00  AX  0   0  4
  [12] .rodata           PROGBITS        0001041c 00041c 00000d 00   A  0   0  4
  [13] .eh_frame_hdr     PROGBITS        0001042c 00042c 000014 00   A  0   0  4
  [14] .eh_frame         PROGBITS        00010440 000440 00002c 00   A  0   0  4
  [15] .preinit_array    PREINIT_ARRAY   00011f04 000f04 000004 04  WA  0   0  1
  [16] .init_array       INIT_ARRAY      00011f08 000f08 000004 04  WA  0   0  4
  [17] .fini_array       FINI_ARRAY      00011f0c 000f0c 000004 04  WA  0   0  4
  [18] .dynamic          DYNAMIC         00011f10 000f10 0000f0 08  WA  6   0  4
  [19] .got              PROGBITS        00012000 001000 000014 04  WA  0   0  4
  [20] .sdata            PROGBITS        00012014 001014 000008 00  WA  0   0  4
  [21] .bss              NOBITS          0001201c 00101c 000004 00  WA  0   0  1
  [22] .comment          PROGBITS        00000000 00101c 000012 01  MS  0   0  1
  [23] .debug_aranges    PROGBITS        00000000 001030 000070 00      0   0  8
  [24] .debug_info       PROGBITS        00000000 0010a0 00063b 00      0   0  1
  [25] .debug_abbrev     PROGBITS        00000000 0016db 00026c 00      0   0  1
  [26] .debug_line       PROGBITS        00000000 001947 00023c 00      0   0  1
  [27] .debug_frame      PROGBITS        00000000 001b84 000054 00      0   0  4
  [28] .debug_str        PROGBITS        00000000 001bd8 00051c 01  MS  0   0  1
  [29] .debug_line_str   PROGBITS        00000000 0020f4 00016b 01  MS  0   0  1
  [30] .debug_loclists   PROGBITS        00000000 00225f 0000b7 00      0   0  1
  [31] .symtab           SYMTAB          00000000 002318 0004a0 10     32  56  4
  [32] .strtab           STRTAB          00000000 0027b8 000231 00      0   0  1
  [33] .shstrtab         STRTAB          00000000 0029e9 00014e 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  D (mbind), p (processor specific)
```

对`hello.o`反汇编，并查看`hello.c`的C程序源码和机器指令的对应关系：

```bash
$ riscv64-unknown-linux-gnu-gcc -g -march=rv32ima -mabi=ilp32 hello.c -o hello.o
$ riscv64-unknown-linux-gnu-objdump -S hello.o

# 此处仅节选main函数内的反汇编结果
0001038c <main>:
#include <stdio.h>

int main(){
   1038c:	ff010113          	addi	sp,sp,-16
   10390:	00112623          	sw	ra,12(sp)
   10394:	00812423          	sw	s0,8(sp)
   10398:	01010413          	addi	s0,sp,16
	printf("hello world!\n");
   1039c:	000107b7          	lui	a5,0x10
   103a0:	41c78513          	addi	a0,a5,1052 # 1041c <__libc_csu_fini+0x4>
   103a4:	f2dff0ef          	jal	ra,102d0 <puts@plt>
	return 0;
   103a8:	00000793          	li	a5,0
}
   103ac:	00078513          	mv	a0,a5
   103b0:	00c12083          	lw	ra,12(sp)
   103b4:	00812403          	lw	s0,8(sp)
   103b8:	01010113          	addi	sp,sp,16
   103bc:	00008067          	ret
```

## 练习4-2

将`hello.c`编译成可调试版本的可执⾏程序`a.out`：

```bash
$ riscv64-unknown-elf-gcc -g -march=rv32i -mabi=ilp32 hello.c
```

先执⾏`qemu-riscv32`运⾏`a.out`：

```bash
$ qemu-riscv32 a.out 
hello world!
```

使⽤`qemu-riscv32`和`gdb`调试`a.out`：

```bash
# 首先编译时需要-g选项带上调试信息
$ riscv64-unknown-elf-gcc -g -march=rv32i -mabi=ilp32 hello.c

# qemu使用调试选项，端口为1234
$ qemu-riscv32 -g 1234 a.out

# 启动gdb
$ riscv64-unknown-elf-gdb
GNU gdb (GDB) 10.1
Copyright (C) 2020 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "--host=x86_64-pc-linux-gnu --target=riscv64-unknown-elf".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<https://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word".

# 加载target为刚刚开放的调试端口
(gdb) target remote localhost:1234
Remote debugging using localhost:1234
warning: No executable has been specified and target does not support
determining executable automatically.  Try using the "file" command.
0x0001008c in ?? ()

# 加载符号表等信息
(gdb) file a.out 
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Reading symbols from a.out...

# 打印源码
(gdb) list
1	#include <stdio.h>
2	
3	int main(){
4		printf("hello world!\n");
5		return 0;
6	}

# 设置断点
(gdb) b 4
Breakpoint 1 at 0x1014c: file hello.c, line 4.

# 运行
(gdb) continue 
Continuing.

Breakpoint 1, main () at hello.c:4
4		printf("hello world!\n");

# 打印寄存器信息
(gdb) info registers 
ra             0x100d4	0x100d4 <_start+72>
sp             0x40800050	0x40800050
gp             0x14e78	0x14e78 <__malloc_av_+984>
tp             0x0	0x0
t0             0x10288	66184
t1             0xf	15
t2             0x0	0
fp             0x40800060	0x40800060
s1             0x0	0
a0             0x1	1
a1             0x40800064	1082130532
a2             0x0	0
a3             0x0	0
a4             0x1	1
a5             0x0	0
a6             0x0	0
a7             0x0	0
s2             0x0	0
s3             0x0	0
s4             0x0	0
s5             0x0	0
s6             0x0	0
s7             0x0	0
--Type <RET> for more, q to quit, c to continue without paging--
s8             0x0	0
s9             0x0	0
s10            0x0	0
s11            0x0	0
t3             0x0	0
t4             0x0	0
t5             0x0	0
t6             0x0	0
pc             0x1014c	0x1014c <main+16>

# 执行至程序结束
(gdb) continue 
Continuing.
[Inferior 1 (process 1) exited normally]

# 此时qemu也会结束并退出
hello world!
$ 
```

这里附上gdb文档手册：

[Debugging with GDB](https://sourceware.org/gdb/onlinedocs/gdb/index.html#SEC_Contents)

## 练习4-3

⾃学Makefile的语法，理解在riscv-operating-system-mooc仓库的根⽬录下执⾏make会发⽣什么。

给出Makefile的官方文档：

[GNU make](https://www.gnu.org/software/make/manual/make.html)

首先查看根目录下的Makefile：

```makefile
SECTIONS = \
	code/asm \
	code/os \

.DEFAULT_GOAL := all
all :
	@echo "begin compile ALL exercises for assembly samples ......................."
	for dir in $(SECTIONS); do $(MAKE) -C $$dir || exit "$$?"; done
	@echo "compile ALL exercises finished successfully! ......"

.PHONY : clean
clean:
	for dir in $(SECTIONS); do $(MAKE) -C $$dir clean || exit "$$?"; done

.PHONY : slides
slides:
	rm -f ./slides/*.pdf
	soffice --headless --convert-to pdf:writer_pdf_Export --outdir ./slides ./docs/ppts/*.pptx
```

首先是定义一个变量`SECTIONS`，其中包含了一个列表，列表中的每一项就是所要进行编译的目录，包括`code/asm`以及`code/os`。

`.DEFAULT_GOAL`定义了默认的目标，也就是make后没有指定目标时，默认执行all目标的命令。

`all`目标中，首先打印`"begin compile ALL exercises for assembly samples ......................."`，然后对于SECTIONS变量中的每个目录，执行`$(MAKE) -C $$dir`。也就是使用当前系统环境定义的make，并切换到对应的目录（`-C`选项）执行对应目录的Makefile选项。如果该命令执行失败，则`exit`退出，并打印最后运行的命令的结束代码（返回值）`$?`。

在Makefile中，`.PHONY`后面的target表示的是一个伪造的target，而不是真实存在的文件target，注意Makefile的target默认是文件。具体区别以及例子可以参见[这里](https://www.cnblogs.com/idorax/p/9306528.html)。
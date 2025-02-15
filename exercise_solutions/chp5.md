# 练习

## 练习5-1

对code/asm/sub执⾏反汇编，查看`sub x5, x6, x7`这条汇编指令对应的机器指令的编码，并对照 RISC-V 的specificaion⾃⼰解析该条指令的编码。

```bash
$ riscv64-unknown-elf-objdump -S test.elf 

test.elf:     file format elf32-littleriscv

Disassembly of section .text:

80000000 <_start>:

	.text			# Define beginning of text section
	.global	_start		# Define entry _start

_start:
	li x6, -1		# x6 = -1
80000000:	fff00313          	li	t1,-1
	li x7, -2		# x7 = -2
80000004:	ffe00393          	li	t2,-2
	sub x5, x6, x7		# x5 = x6 - x7
80000008:	407302b3          	sub	t0,t1,t2

8000000c <stop>:

stop:
	j stop			# Infinite loop to stop execution
8000000c:	0000006f          	j	8000000c <stop>
```

可以看出对应的指令编码为`407302b3`：

```bash
0x407302b3 -> 0b0100_0000_0111_0011_0000_0010_1011_0011
按照域划分：0100000 00111 00110 000 00101 0110011
opcode = 0110011 -> OP
rd     = 00101   -> x5
funct3 = 000     
rs1    = 00110   -> x6
rs2    = 00111   -> x7
funct7 = 0100000 + funct3 -> SUB
所以该指令就是 sub x5, x6, x7
```

现知道某条RISC-V的机器指令在内存中的值为 `b3 05 95 00`，从左往右为从低地址到⾼地址，单位为字节，请将其翻译为对应的汇编指令。

```bash
0x009505b3 -> 0b0000_0000_1001_0101_0000_0101_1011_0011
按照域划分：0000000 01001 01010 000 01011 0110011
opcode = 0110011 -> OP
rd     = 01011   -> x11
funct3 = 000     
rs1    = 01010   -> x10
rs2    = 01001   -> x9
funct7 = 0000000 + funct3 -> ADD
所以该指令是ADD x11, x9, x10
```

## 练习5-2

假设有如下这么⼀段C语⾔程序代码，尝试编写⼀段汇编代码，达到等效的结果，并采⽤ gdb 调试查看执⾏结果，注意请使⽤寄存器来存放变量的值：

```c
register int a, b, c, d, e; 
b = 1; 
c = 2; 
e = 3; 
a = b + c; 
d = a - e;
```

编写汇编代码`test.s`：

```
# code/asm/exer52/test.s

# Correspoding C program:
# register int a, b, c, d, e; 
# b = 1; 
# c = 2; 
# e = 3; 
# a = b + c; 
# d = a - e;

    .text
    .global _start

_start:
    li x6, 1        # b = 1;
    li x7, 2        # c = 2;
    li x9, 3        # e = 3;
    add x5, x6, x7  # a = b + c;
    sub x8, x5, x9  # d = a - e;

stop:
    j stop

    .end
```

重用课程已经带有的Make规则，不重复造轮子，只是需要稍微修改一下Makefile以及gdbinit：

```makefile
# code/asm/exer52/Makefile

EXEC = test

SRC = ${EXEC}.s

GDBINIT = ./gdbinit

include ../rule.mk
```

```
# code/asm/exer52/gdbinit

display/z $x5
display/z $x6
display/z $x7
display/z $x8
display/z $x9

set disassemble-next-line on
b _start
target remote : 1234
c
```

编译：

```bash
$ make
test.s: Assembler messages:
test.s: Warning: end of file not at end of a line; newline inserted
```

调试：

```bash
$ make debug
test.s: Assembler messages:
test.s: Warning: end of file not at end of a line; newline inserted
Press Ctrl-C and then input 'quit' to exit GDB and QEMU
-------------------------------------------------------
Reading symbols from test.elf...
Breakpoint 1 at 0x80000000: file test.s, line 13.
0x00001000 in ?? ()
=> 0x00001000:  97 02 00 00     auipc   t0,0x0
1: /z $x5 = 0x00000000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000000

Breakpoint 1, _start () at test.s:13
13          li x6, 1        # b = 1;
=> 0x80000000 <_start+0>:       13 03 10 00     li      t1,1
--Type <RET> for more, q to quit, c to continue without paging--
1: /z $x5 = 0x80000000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000000
(gdb) si
14          li x7, 2        # c = 2;
=> 0x80000004 <_start+4>:       93 03 20 00     li      t2,2
1: /z $x5 = 0x80000000
2: /z $x6 = 0x00000001
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000000
(gdb) 
15          li x9, 3        # e = 3;
=> 0x80000008 <_start+8>:       93 04 30 00     li      s1,3
1: /z $x5 = 0x80000000
2: /z $x6 = 0x00000001
3: /z $x7 = 0x00000002
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000000
(gdb) 
16          add x5, x6, x7  # a = b + c;
=> 0x8000000c <_start+12>:      b3 02 73 00     add     t0,t1,t2
1: /z $x5 = 0x80000000
2: /z $x6 = 0x00000001
3: /z $x7 = 0x00000002
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000003
(gdb) 
17          sub x8, x5, x9  # d = a - e;
=> 0x80000010 <_start+16>:      33 84 92 40     sub     s0,t0,s1
1: /z $x5 = 0x00000003
2: /z $x6 = 0x00000001
3: /z $x7 = 0x00000002
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000003
(gdb) 
stop () at test.s:20
20          j stop
=> 0x80000014 <stop+0>: 6f 00 00 00     j       0x80000014 <stop>
1: /z $x5 = 0x00000003
2: /z $x6 = 0x00000001
3: /z $x7 = 0x00000002
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000003
(gdb) q
A debugging session is active.

        Inferior 1 [process 1] will be detached.

Quit anyway? (y or n) y
Detaching from program: /home/server/Documents/riscv-operating-system-mooc/code/asm/exer52/test.elf, process 1
Ending remote debugging.
[Inferior 1 (process 1) detached]
```

## 练习5-3

假设有如下这么⼀段C语⾔程序代码，尝试编写⼀段汇编代码，达到等效的结果，并采⽤ gdb 调试查看执⾏结果，注意请使⽤寄存器来存放变量的值：

```c
register int a, b, c, d, e;
b = 1;
c = 2;
d = 3;
e = 4;
a = (b + c) - (d + e);
```

汇编代码`test.s`：

```
# Correspoding C program:
# register int a, b, c, d, e;
# b = 1;
# c = 2;
# d = 3;
# e = 4;
# a = (b + c) - (d + e);

    .text
    .global _start

_start:
    li x6, 1        # b = 1;
    li x7, 2        # c = 2;
    li x8, 3        # d = 3;
    li x9, 4        # e = 4;
    add x5, x6, x7  # a = b + c
    add x10, x8, x9 # tmp = d + e
    sub x5, x5, x10 # a = a - tmp

stop:
    j stop

    .end
```

调试：

```bash
$ make debug
test.s: Assembler messages:
test.s: Warning: end of file not at end of a line; newline inserted
Press Ctrl-C and then input 'quit' to exit GDB and QEMU
-------------------------------------------------------
Reading symbols from test.elf...
Breakpoint 1 at 0x80000000: file test.s, line 13.
0x00001000 in ?? ()
=> 0x00001000:  97 02 00 00     auipc   t0,0x0
1: /z $x5 = 0x00000000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000000
6: /z $x10 = 0x00000000

--Type <RET> for more, q to quit, c to continue without paging--
Breakpoint 1, _start () at test.s:13
13          li x6, 1        # b = 1;
=> 0x80000000 <_start+0>:       13 03 10 00     li      t1,1
1: /z $x5 = 0x80000000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000000
6: /z $x10 = 0x00000000
(gdb) si
14          li x7, 2        # c = 2;
=> 0x80000004 <_start+4>:       93 03 20 00     li      t2,2
1: /z $x5 = 0x80000000
2: /z $x6 = 0x00000001
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000000
6: /z $x10 = 0x00000000
(gdb) 
15          li x8, 3        # d = 3;
=> 0x80000008 <_start+8>:       13 04 30 00     li      s0,3
1: /z $x5 = 0x80000000
2: /z $x6 = 0x00000001
3: /z $x7 = 0x00000002
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000000
6: /z $x10 = 0x00000000
(gdb) 
16          li x9, 4        # e = 4;
=> 0x8000000c <_start+12>:      93 04 40 00     li      s1,4
1: /z $x5 = 0x80000000
2: /z $x6 = 0x00000001
3: /z $x7 = 0x00000002
4: /z $x8 = 0x00000003
5: /z $x9 = 0x00000000
6: /z $x10 = 0x00000000
(gdb) 
17          add x5, x6, x7  # a = b + c
=> 0x80000010 <_start+16>:      b3 02 73 00     add     t0,t1,t2
1: /z $x5 = 0x80000000
2: /z $x6 = 0x00000001
3: /z $x7 = 0x00000002
4: /z $x8 = 0x00000003
5: /z $x9 = 0x00000004
6: /z $x10 = 0x00000000
(gdb) 
18          add x10, x8, x9 # tmp = d + e
=> 0x80000014 <_start+20>:      33 05 94 00     add     a0,s0,s1
1: /z $x5 = 0x00000003
2: /z $x6 = 0x00000001
3: /z $x7 = 0x00000002
4: /z $x8 = 0x00000003
5: /z $x9 = 0x00000004
6: /z $x10 = 0x00000000
(gdb) 
19          sub x5, x5, x10 # a = a - tmp
=> 0x80000018 <_start+24>:      b3 82 a2 40     sub     t0,t0,a0
1: /z $x5 = 0x00000003
2: /z $x6 = 0x00000001
3: /z $x7 = 0x00000002
4: /z $x8 = 0x00000003
5: /z $x9 = 0x00000004
6: /z $x10 = 0x00000007
(gdb) 
stop () at test.s:22
22          j stop
=> 0x8000001c <stop+0>: 6f 00 00 00     j       0x8000001c <stop>
1: /z $x5 = 0xfffffffc
2: /z $x6 = 0x00000001
3: /z $x7 = 0x00000002
4: /z $x8 = 0x00000003
5: /z $x9 = 0x00000004
6: /z $x10 = 0x00000007
(gdb) 
22          j stop
=> 0x8000001c <stop+0>: 6f 00 00 00     j       0x8000001c <stop>
1: /z $x5 = 0xfffffffc
2: /z $x6 = 0x00000001
3: /z $x7 = 0x00000002
4: /z $x8 = 0x00000003
5: /z $x9 = 0x00000004
6: /z $x10 = 0x00000007
(gdb) q
A debugging session is active.

        Inferior 1 [process 1] will be detached.

Quit anyway? (y or n) y
Detaching from program: /home/server/Documents/riscv-operating-system-mooc/code/asm/exer53/test.elf, process 1
Ending remote debugging.
[Inferior 1 (process 1) detached]
```

## 练习5-4

给定⼀个 32 位数 0x87654321，先编写 c 程序，将其低 16 位 (0x4321) 和⾼ 16 位 (0x8765) 分别分离出来保存 到独立的变量中；完成后再尝试采⽤汇编语⾔实现类似的效果。

C程序test.c如下：

```c
#include <stdio.h>

int main() {
    int a = 0x87654321;
    unsigned int low = a & 0x0000ffff;
    unsigned int high = a & 0xffff0000;
    high = high >> 16;

    printf("low = 0x%x, high = 0x%x\n", low, high);
    return 0;
}
```

编译运行：

```bash
$ gcc test.c -o test
$ ./test
low = 0x4321, high = 0x8765
```

汇编代码test.s如下：

```
.text
    .global _start

_start:
    li x5, 0x87654321       # x5 = 0x87654321
    li x6, 0x0000ffff       # x6 = 0x0000ffff
    li x7, 0xffff0000       # x7 = 0xffff0000
    and x8, x5, x6          # x8 = 0x00004321
    and x9, x5, x7          # x9 = 0x87650000
    srli x9, x9, 16         # x9 = 0x00008765

stop:
    j stop

    .end
```

调试运行：

```bash
$ make debug
test.s: Assembler messages:
test.s: Warning: end of file not at end of a line; newline inserted
Press Ctrl-C and then input 'quit' to exit GDB and QEMU
-------------------------------------------------------
Reading symbols from test.elf...
Breakpoint 1 at 0x80000000: file test.s, line 6.
0x00001000 in ?? ()
=> 0x00001000:  97 02 00 00     auipc   t0,0x0
1: /z $x5 = 0x00000000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000000

Breakpoint 1, _start () at test.s:6
6           li x5, 0x87654321       # x5 = 0x87654321
=> 0x80000000 <_start+0>:       b7 42 65 87     lui     t0,0x87654
   0x80000004 <_start+4>:       93 82 12 32     addi    t0,t0,801 # 0x87654321
1: /z $x5 = 0x80000000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000000
(gdb) si
0x80000004      6           li x5, 0x87654321       # x5 = 0x87654321
   0x80000000 <_start+0>:       b7 42 65 87     lui     t0,0x87654
=> 0x80000004 <_start+4>:       93 82 12 32     addi    t0,t0,801 # 0x87654321
1: /z $x5 = 0x87654000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000000
(gdb) 
7           li x6, 0x0000ffff       # x6 = 0x0000ffff
=> 0x80000008 <_start+8>:       37 03 01 00     lui     t1,0x10
   0x8000000c <_start+12>:      13 03 f3 ff     addi    t1,t1,-1 # 0xffff
1: /z $x5 = 0x87654321
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000000
(gdb) 
0x8000000c      7           li x6, 0x0000ffff       # x6 = 0x0000ffff
   0x80000008 <_start+8>:       37 03 01 00     lui     t1,0x10
=> 0x8000000c <_start+12>:      13 03 f3 ff     addi    t1,t1,-1 # 0xffff
1: /z $x5 = 0x87654321
2: /z $x6 = 0x00010000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000000
(gdb) 
8           li x7, 0xffff0000       # x7 = 0xffff0000
=> 0x80000010 <_start+16>:      b7 03 ff ff     lui     t2,0xffff0
1: /z $x5 = 0x87654321
2: /z $x6 = 0x0000ffff
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000000
(gdb) 
9           and x8, x5, x6          # x8 = 0x00004321
=> 0x80000014 <_start+20>:      33 f4 62 00     and     s0,t0,t1
1: /z $x5 = 0x87654321
2: /z $x6 = 0x0000ffff
3: /z $x7 = 0xffff0000
4: /z $x8 = 0x00000000
5: /z $x9 = 0x00000000
(gdb) 
10          and x9, x5, x7          # x9 = 0x87650000
=> 0x80000018 <_start+24>:      b3 f4 72 00     and     s1,t0,t2
1: /z $x5 = 0x87654321
2: /z $x6 = 0x0000ffff
3: /z $x7 = 0xffff0000
4: /z $x8 = 0x00004321
5: /z $x9 = 0x00000000
(gdb) 
11          srli x9, x9, 16
=> 0x8000001c <_start+28>:      93 d4 04 01     srli    s1,s1,0x10
1: /z $x5 = 0x87654321
2: /z $x6 = 0x0000ffff
3: /z $x7 = 0xffff0000
4: /z $x8 = 0x00004321
5: /z $x9 = 0x87650000
(gdb) 
stop () at test.s:14
14          j stop
=> 0x80000020 <stop+0>: 6f 00 00 00     j       0x80000020 <stop>
1: /z $x5 = 0x87654321
2: /z $x6 = 0x0000ffff
3: /z $x7 = 0xffff0000
4: /z $x8 = 0x00004321
5: /z $x9 = 0x00008765
(gdb) 
14          j stop
=> 0x80000020 <stop+0>: 6f 00 00 00     j       0x80000020 <stop>
1: /z $x5 = 0x87654321
2: /z $x6 = 0x0000ffff
3: /z $x7 = 0xffff0000
4: /z $x8 = 0x00004321
5: /z $x9 = 0x00008765
(gdb) q
A debugging session is active.

        Inferior 1 [process 1] will be detached.

Quit anyway? (y or n) y
Detaching from program: /home/server/Documents/riscv-operating-system-mooc/code/asm/exer54/test.elf, process 1
Ending remote debugging.
[Inferior 1 (process 1) detached]
```

这里可以看出，汇编伪指令`li`会根据所要load的立即数采用不同的策略生成不同的机器代码。通过反汇编可以看到：

```bash
$ riscv64-unknown-elf-objdump -S test.elf 

test.elf:     file format elf32-littleriscv

Disassembly of section .text:

80000000 <_start>:
riscv64-unknown-elf-objdump: Warning: source file /home/server/Documents/riscv-operating-system-mooc/code/asm/exer54/test.s is more recent than object file

    .text
    .global _start

_start:
    li x5, 0x87654321       # x5 = 0x87654321
80000000:       876542b7                lui     t0,0x87654
80000004:       32128293                addi    t0,t0,801 # 87654321 <__global_pointer$+0x7652afd>
    li x6, 0x0000ffff       # x6 = 0x0000ffff
80000008:       00010337                lui     t1,0x10
8000000c:       fff30313                addi    t1,t1,-1 # ffff <_start-0x7fff0001>
    li x7, 0xffff0000       # x7 = 0xffff0000
80000010:       ffff03b7                lui     t2,0xffff0
    and x8, x5, x6          # x8 = 0x00004321
80000014:       0062f433                and     s0,t0,t1
    and x9, x5, x7          # x9 = 0x87650000
80000018:       0072f4b3                and     s1,t0,t2
    srli x9, x9, 16         # x9 = 0x00008765
8000001c:       0104d493                srli    s1,s1,0x10

80000020 <stop>:

stop:
    j stop
80000020:       0000006f                j       80000020 <stop>
```

## 练习5-5

假设有如下这么⼀段 C 代码：

```c
int array[2] = {0x11111111, 0xffffffff};
int i = array[0];
int j = array[1];
```

尝试⽤汇编语句实现等效的功能。编写汇编代码`test.s`：

```
# Correspoding C program:
# int array[2] = {0x11111111, 0xffffffff};
# int i = array[0];
# int j = array[1];

    .text
    .global _start

_start:
    la x5, _array
    lw x6, 0(x5)        # int i = array[0]
    lw x7, 4(x5)        # int j = array[1]

stop:
    j stop

_array:
    .word 0x11111111
    .word 0xffffffff

    .end
```

调试：

```bash
$ make debug
Press Ctrl-C and then input 'quit' to exit GDB and QEMU
-------------------------------------------------------
Reading symbols from test.elf...
Breakpoint 1 at 0x80000000: file test.s, line 10.
0x00001000 in ?? ()
=> 0x00001000:  97 02 00 00     auipc   t0,0x0
1: /z $x5 = 0x00000000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000

Breakpoint 1, _start () at test.s:10
10          la x5, _array
=> 0x80000000 <_start+0>:       97 02 00 00     auipc   t0,0x0
   0x80000004 <_start+4>:       93 82 42 01     addi    t0,t0,20 # 0x80000014 <_array>
1: /z $x5 = 0x80000000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
(gdb) si
0x80000004      10          la x5, _array
   0x80000000 <_start+0>:       97 02 00 00     auipc   t0,0x0
=> 0x80000004 <_start+4>:       93 82 42 01     addi    t0,t0,20 # 0x80000014 <_array>
1: /z $x5 = 0x80000000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
(gdb) 
11          lw x6, 0(x5)        # int i = array[0]
=> 0x80000008 <_start+8>:       03 a3 02 00     lw      t1,0(t0)
1: /z $x5 = 0x80000014
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
(gdb) 
12          lw x7, 4(x5)        # int j = array[1]
=> 0x8000000c <_start+12>:      83 a3 42 00     lw      t2,4(t0)
1: /z $x5 = 0x80000014
2: /z $x6 = 0x11111111
3: /z $x7 = 0x00000000
(gdb) 
stop () at test.s:15
15          j stop
=> 0x80000010 <stop+0>: 6f 00 00 00     j       0x80000010 <stop>
   0x80000014 <_array+0>:       11 11   addi    sp,sp,-28
   0x80000016 <_array+2>:       11 11   addi    sp,sp,-28
   0x80000018 <_array+4>:       ff ff   0xffff
   0x8000001a <_array+6>:       ff ff   0xffff
1: /z $x5 = 0x80000014
2: /z $x6 = 0x11111111
3: /z $x7 = 0xffffffff
(gdb) q
A debugging session is active.

        Inferior 1 [process 1] will be detached.

Quit anyway? (y or n) y
Detaching from program: /home/server/Documents/riscv-operating-system-mooc/code/asm/exer55/test.elf, process 1
Ending remote debugging.
[Inferior 1 (process 1) detached]
```

## 练习5-6

在内存中定义⼀个结构体变量，编写汇编程序，⽤宏⽅式（.macro/.endm）实现对结构体变量的成员赋值以及 读取该结构体变量的成员的值到寄存器变量中。等价的 c 语⾔的⽰例代码如下，供参考：

```c
struct S { unsigned int a; unsigned int b; };
struct S s = {0};
#define set_struct(s) \
s.a = a; \
s.b = b;
#define get_struct(s) \
a = s.a; \
b = s.b;
void foo() {
	register unsigned int a = 0x12345678; 
	register unsigned int b = 0x87654321; 
	set_struct(s);
	a = 0;
	b = 0;
	get_struct(s);
}
```

编写汇编代码`test.s`：

```
# Correspoding C program:
# struct S { unsigned int a; unsigned int b; };
# struct S s = {0};
# #define set_struct(s) \
# s.a = a; \
# s.b = b;
# #define get_struct(s) \
# a = s.a; \
# b = s.b;
# void foo() {
# register unsigned int a = 0x12345678; 
# register unsigned int b = 0x87654321; 
# set_struct(s);
# a = 0;
# b = 0;
# get_struct(s);
# }
# a is in x5, b is in x6, s.a is in x7, s.b is in x8

    .text
    .global _start

.macro set_struct
    mv x7, x5
    mv x8, x6
.endm

.macro get_struct
    mv x5, x7
    mv x6, x8
.endm

_start:
    li x5, 0x12345678       # a = 0x12345678
    li x6, 0x87654321       # b = 0x87654321
    set_struct
    li x5, 0
    li x6, 0
    get_struct

stop:
    j stop

    .end
```

调试：

```bash
$ make debug
Press Ctrl-C and then input 'quit' to exit GDB and QEMU
-------------------------------------------------------
Reading symbols from test.elf...
Breakpoint 1 at 0x80000000: file test.s, line 34.
0x00001000 in ?? ()
=> 0x00001000:  97 02 00 00     auipc   t0,0x0
1: /z $x5 = 0x00000000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
--Type <RET> for more, q to quit, c to continue without paging--

Breakpoint 1, _start () at test.s:34
34          li x5, 0x12345678       # a = 0x12345678
=> 0x80000000 <_start+0>:       b7 52 34 12     lui     t0,0x12345
   0x80000004 <_start+4>:       93 82 82 67     addi    t0,t0,1656 # 0x12345678
1: /z $x5 = 0x80000000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
(gdb) si
0x80000004      34          li x5, 0x12345678       # a = 0x12345678
   0x80000000 <_start+0>:       b7 52 34 12     lui     t0,0x12345
=> 0x80000004 <_start+4>:       93 82 82 67     addi    t0,t0,1656 # 0x12345678
1: /z $x5 = 0x12345000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
(gdb) 
35          li x6, 0x87654321       # b = 0x87654321
=> 0x80000008 <_start+8>:       37 43 65 87     lui     t1,0x87654
   0x8000000c <_start+12>:      13 03 13 32     addi    t1,t1,801 # 0x87654321
1: /z $x5 = 0x12345678
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
(gdb) 
0x8000000c      35          li x6, 0x87654321       # b = 0x87654321
   0x80000008 <_start+8>:       37 43 65 87     lui     t1,0x87654
=> 0x8000000c <_start+12>:      13 03 13 32     addi    t1,t1,801 # 0x87654321
1: /z $x5 = 0x12345678
2: /z $x6 = 0x87654000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
(gdb) 
36          set_struct
=> 0x80000010 <_start+16>:      93 83 02 00     mv      t2,t0
   0x80000014 <_start+20>:      13 04 03 00     mv      s0,t1
1: /z $x5 = 0x12345678
2: /z $x6 = 0x87654321
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
(gdb) 
0x80000014      36          set_struct
   0x80000010 <_start+16>:      93 83 02 00     mv      t2,t0
=> 0x80000014 <_start+20>:      13 04 03 00     mv      s0,t1
1: /z $x5 = 0x12345678
2: /z $x6 = 0x87654321
3: /z $x7 = 0x12345678
4: /z $x8 = 0x00000000
(gdb) 
37          li x5, 0
=> 0x80000018 <_start+24>:      93 02 00 00     li      t0,0
1: /z $x5 = 0x12345678
2: /z $x6 = 0x87654321
3: /z $x7 = 0x12345678
4: /z $x8 = 0x87654321
(gdb) 
38          li x6, 0
=> 0x8000001c <_start+28>:      13 03 00 00     li      t1,0
1: /z $x5 = 0x00000000
2: /z $x6 = 0x87654321
3: /z $x7 = 0x12345678
4: /z $x8 = 0x87654321
(gdb) 
39          get_struct
=> 0x80000020 <_start+32>:      93 82 03 00     mv      t0,t2
   0x80000024 <_start+36>:      13 03 04 00     mv      t1,s0
1: /z $x5 = 0x00000000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x12345678
4: /z $x8 = 0x87654321
(gdb) 
0x80000024      39          get_struct
   0x80000020 <_start+32>:      93 82 03 00     mv      t0,t2
=> 0x80000024 <_start+36>:      13 03 04 00     mv      t1,s0
1: /z $x5 = 0x12345678
2: /z $x6 = 0x00000000
3: /z $x7 = 0x12345678
4: /z $x8 = 0x87654321
(gdb) 
stop () at test.s:42
42          j stop
=> 0x80000028 <stop+0>: 6f 00 00 00     j       0x80000028 <stop>
1: /z $x5 = 0x12345678
2: /z $x6 = 0x87654321
3: /z $x7 = 0x12345678
4: /z $x8 = 0x87654321
(gdb) q
A debugging session is active.

        Inferior 1 [process 1] will be detached.

Quit anyway? (y or n) y
Detaching from program: /home/server/Documents/riscv-operating-system-mooc/code/asm/exer56/test.elf, process 1
Ending remote debugging.
[Inferior 1 (process 1) detached]
```

## 练习5-7

编写汇编指令，使⽤条件分⽀指令循环遍历⼀个字符串数组， 获取该字符串的⻓度。等价的 c 语⾔的⽰例代码 如下，供参考：

```c
char array[] = {'h', 'e', 'l', 'l', 'o', ',', 'w', 'o', 'r', 'l', 'd', '!', '\0'};
int len = 0;
while (array[len] != '\0') { len++; }
```

汇编代码如下`test.s`：

```
# Correspoding C program:
# char array[] = {'h', 'e', 'l', 'l', 'o', ',', 'w', 'o', 'r', 'l', 'd', '!', '\0'};
# int len = 0;
# while (array[len] != '\0') { len++; }

    .text
    .global _start

_start:
    li x5, 0        # len = 0
    la x6, _array
    lb x7, 0(x6)
    li x8, '\0'
    j judge
acc:
    addi x5, x5, 1
    addi x6, x6, 1
    lb x7, 0(x6)
judge:
    bne x7, x8, acc

stop:
    j stop

_array:
    .byte 'h'
    .byte 'e'
    .byte 'l'
    .byte 'l'
    .byte 'o'
    .byte ','
    .byte 'w'
    .byte 'o'
    .byte 'r'
    .byte 'l'
    .byte 'd'
    .byte '!'
    .byte '\0'

    .end
```

调试：

```bash
$ make debug
Press Ctrl-C and then input 'quit' to exit GDB and QEMU
-------------------------------------------------------
Reading symbols from test.elf...
Breakpoint 1 at 0x80000000: file test.s, line 10.
0x00001000 in ?? ()
=> 0x00001000:  97 02 00 00     auipc   t0,0x0
1: /z $x5 = 0x00000000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000

Breakpoint 1, _start () at test.s:10
10          li x5, 0        # len = 0
=> 0x80000000 <_start+0>:       93 02 00 00     li      t0,0
1: /z $x5 = 0x80000000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
(gdb) si
11          la x6, _array
=> 0x80000004 <_start+4>:       17 03 00 00     auipc   t1,0x0
   0x80000008 <_start+8>:       13 03 83 02     addi    t1,t1,40 # 0x8000002c <_array>
1: /z $x5 = 0x00000000
2: /z $x6 = 0x00000000
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
(gdb) 
0x80000008      11          la x6, _array
   0x80000004 <_start+4>:       17 03 00 00     auipc   t1,0x0
=> 0x80000008 <_start+8>:       13 03 83 02     addi    t1,t1,40 # 0x8000002c <_array>
1: /z $x5 = 0x00000000
2: /z $x6 = 0x80000004
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
(gdb) 
12          lb x7, 0(x6)
=> 0x8000000c <_start+12>:      83 03 03 00     lb      t2,0(t1)
1: /z $x5 = 0x00000000
2: /z $x6 = 0x8000002c
3: /z $x7 = 0x00000000
4: /z $x8 = 0x00000000
(gdb) 
13          li x8, '\0'
=> 0x80000010 <_start+16>:      13 04 00 03     li      s0,48
1: /z $x5 = 0x00000000
2: /z $x6 = 0x8000002c
3: /z $x7 = 0x00000068
4: /z $x8 = 0x00000000
(gdb) 
14          j judge
=> 0x80000014 <_start+20>:      6f 00 00 01     j       0x80000024 <judge>
1: /z $x5 = 0x00000000
2: /z $x6 = 0x8000002c
3: /z $x7 = 0x00000068
4: /z $x8 = 0x00000030
(gdb) 
judge () at test.s:20
20          bne x7, x8, acc
=> 0x80000024 <judge+0>:        e3 9a 83 fe     bne     t2,s0,0x80000018 <acc>
1: /z $x5 = 0x00000000
2: /z $x6 = 0x8000002c
3: /z $x7 = 0x00000068
4: /z $x8 = 0x00000030
(gdb) 
acc () at test.s:16
16          addi x5, x5, 1
=> 0x80000018 <acc+0>:  93 82 12 00     addi    t0,t0,1
1: /z $x5 = 0x00000000
2: /z $x6 = 0x8000002c
3: /z $x7 = 0x00000068
4: /z $x8 = 0x00000030
(gdb) 
17          addi x6, x6, 1
=> 0x8000001c <acc+4>:  13 03 13 00     addi    t1,t1,1
1: /z $x5 = 0x00000001
2: /z $x6 = 0x8000002c
3: /z $x7 = 0x00000068
4: /z $x8 = 0x00000030
(gdb) 
18          lb x7, 0(x6)
=> 0x80000020 <acc+8>:  83 03 03 00     lb      t2,0(t1)
1: /z $x5 = 0x00000001
2: /z $x6 = 0x8000002d
3: /z $x7 = 0x00000068
4: /z $x8 = 0x00000030
(gdb) 
judge () at test.s:20
20          bne x7, x8, acc
=> 0x80000024 <judge+0>:        e3 9a 83 fe     bne     t2,s0,0x80000018 <acc>
1: /z $x5 = 0x00000001
2: /z $x6 = 0x8000002d
3: /z $x7 = 0x00000065
4: /z $x8 = 0x00000030
(gdb) 
acc () at test.s:16
16          addi x5, x5, 1
=> 0x80000018 <acc+0>:  93 82 12 00     addi    t0,t0,1
1: /z $x5 = 0x00000001
2: /z $x6 = 0x8000002d
3: /z $x7 = 0x00000065
4: /z $x8 = 0x00000030
(gdb) 
17          addi x6, x6, 1
=> 0x8000001c <acc+4>:  13 03 13 00     addi    t1,t1,1
1: /z $x5 = 0x00000002
2: /z $x6 = 0x8000002d
3: /z $x7 = 0x00000065
4: /z $x8 = 0x00000030
(gdb) 
18          lb x7, 0(x6)
=> 0x80000020 <acc+8>:  83 03 03 00     lb      t2,0(t1)
1: /z $x5 = 0x00000002
2: /z $x6 = 0x8000002e
3: /z $x7 = 0x00000065
4: /z $x8 = 0x00000030
(gdb) 
judge () at test.s:20
20          bne x7, x8, acc
=> 0x80000024 <judge+0>:        e3 9a 83 fe     bne     t2,s0,0x80000018 <acc>
1: /z $x5 = 0x00000002
2: /z $x6 = 0x8000002e
3: /z $x7 = 0x0000006c
4: /z $x8 = 0x00000030
(gdb) 
acc () at test.s:16
16          addi x5, x5, 1
=> 0x80000018 <acc+0>:  93 82 12 00     addi    t0,t0,1
1: /z $x5 = 0x00000002
2: /z $x6 = 0x8000002e
3: /z $x7 = 0x0000006c
4: /z $x8 = 0x00000030
(gdb) 
17          addi x6, x6, 1
=> 0x8000001c <acc+4>:  13 03 13 00     addi    t1,t1,1
1: /z $x5 = 0x00000003
2: /z $x6 = 0x8000002e
3: /z $x7 = 0x0000006c
4: /z $x8 = 0x00000030
(gdb) 
18          lb x7, 0(x6)
=> 0x80000020 <acc+8>:  83 03 03 00     lb      t2,0(t1)
1: /z $x5 = 0x00000003
2: /z $x6 = 0x8000002f
3: /z $x7 = 0x0000006c
4: /z $x8 = 0x00000030
(gdb) 
judge () at test.s:20
20          bne x7, x8, acc
=> 0x80000024 <judge+0>:        e3 9a 83 fe     bne     t2,s0,0x80000018 <acc>
1: /z $x5 = 0x00000003
2: /z $x6 = 0x8000002f
3: /z $x7 = 0x0000006c
4: /z $x8 = 0x00000030
(gdb) 
acc () at test.s:16
16          addi x5, x5, 1
=> 0x80000018 <acc+0>:  93 82 12 00     addi    t0,t0,1
1: /z $x5 = 0x00000003
2: /z $x6 = 0x8000002f
3: /z $x7 = 0x0000006c
4: /z $x8 = 0x00000030
(gdb) 
17          addi x6, x6, 1
=> 0x8000001c <acc+4>:  13 03 13 00     addi    t1,t1,1
1: /z $x5 = 0x00000004
2: /z $x6 = 0x8000002f
3: /z $x7 = 0x0000006c
4: /z $x8 = 0x00000030
(gdb) 
18          lb x7, 0(x6)
=> 0x80000020 <acc+8>:  83 03 03 00     lb      t2,0(t1)
1: /z $x5 = 0x00000004
2: /z $x6 = 0x80000030
3: /z $x7 = 0x0000006c
4: /z $x8 = 0x00000030
(gdb) 
judge () at test.s:20
20          bne x7, x8, acc
=> 0x80000024 <judge+0>:        e3 9a 83 fe     bne     t2,s0,0x80000018 <acc>
1: /z $x5 = 0x00000004
2: /z $x6 = 0x80000030
3: /z $x7 = 0x0000006f
4: /z $x8 = 0x00000030
(gdb) 
acc () at test.s:16
16          addi x5, x5, 1
=> 0x80000018 <acc+0>:  93 82 12 00     addi    t0,t0,1
1: /z $x5 = 0x00000004
2: /z $x6 = 0x80000030
3: /z $x7 = 0x0000006f
4: /z $x8 = 0x00000030
(gdb) 
17          addi x6, x6, 1
=> 0x8000001c <acc+4>:  13 03 13 00     addi    t1,t1,1
1: /z $x5 = 0x00000005
2: /z $x6 = 0x80000030
3: /z $x7 = 0x0000006f
4: /z $x8 = 0x00000030
(gdb) 
18          lb x7, 0(x6)
=> 0x80000020 <acc+8>:  83 03 03 00     lb      t2,0(t1)
1: /z $x5 = 0x00000005
2: /z $x6 = 0x80000031
3: /z $x7 = 0x0000006f
4: /z $x8 = 0x00000030
(gdb) 
judge () at test.s:20
20          bne x7, x8, acc
=> 0x80000024 <judge+0>:        e3 9a 83 fe     bne     t2,s0,0x80000018 <acc>
1: /z $x5 = 0x00000005
2: /z $x6 = 0x80000031
3: /z $x7 = 0x0000002c
4: /z $x8 = 0x00000030
(gdb) 
acc () at test.s:16
16          addi x5, x5, 1
=> 0x80000018 <acc+0>:  93 82 12 00     addi    t0,t0,1
1: /z $x5 = 0x00000005
2: /z $x6 = 0x80000031
3: /z $x7 = 0x0000002c
4: /z $x8 = 0x00000030
(gdb) 
17          addi x6, x6, 1
=> 0x8000001c <acc+4>:  13 03 13 00     addi    t1,t1,1
1: /z $x5 = 0x00000006
2: /z $x6 = 0x80000031
3: /z $x7 = 0x0000002c
4: /z $x8 = 0x00000030
(gdb) 
18          lb x7, 0(x6)
=> 0x80000020 <acc+8>:  83 03 03 00     lb      t2,0(t1)
1: /z $x5 = 0x00000006
2: /z $x6 = 0x80000032
3: /z $x7 = 0x0000002c
4: /z $x8 = 0x00000030
(gdb) 
judge () at test.s:20
20          bne x7, x8, acc
=> 0x80000024 <judge+0>:        e3 9a 83 fe     bne     t2,s0,0x80000018 <acc>
1: /z $x5 = 0x00000006
2: /z $x6 = 0x80000032
3: /z $x7 = 0x00000077
4: /z $x8 = 0x00000030
(gdb) 
acc () at test.s:16
16          addi x5, x5, 1
=> 0x80000018 <acc+0>:  93 82 12 00     addi    t0,t0,1
1: /z $x5 = 0x00000006
2: /z $x6 = 0x80000032
3: /z $x7 = 0x00000077
4: /z $x8 = 0x00000030
(gdb) 
17          addi x6, x6, 1
=> 0x8000001c <acc+4>:  13 03 13 00     addi    t1,t1,1
1: /z $x5 = 0x00000007
2: /z $x6 = 0x80000032
3: /z $x7 = 0x00000077
4: /z $x8 = 0x00000030
(gdb) 
18          lb x7, 0(x6)
=> 0x80000020 <acc+8>:  83 03 03 00     lb      t2,0(t1)
1: /z $x5 = 0x00000007
2: /z $x6 = 0x80000033
3: /z $x7 = 0x00000077
4: /z $x8 = 0x00000030
(gdb) 
judge () at test.s:20
20          bne x7, x8, acc
=> 0x80000024 <judge+0>:        e3 9a 83 fe     bne     t2,s0,0x80000018 <acc>
1: /z $x5 = 0x00000007
2: /z $x6 = 0x80000033
3: /z $x7 = 0x0000006f
4: /z $x8 = 0x00000030
(gdb) 
acc () at test.s:16
16          addi x5, x5, 1
=> 0x80000018 <acc+0>:  93 82 12 00     addi    t0,t0,1
1: /z $x5 = 0x00000007
2: /z $x6 = 0x80000033
3: /z $x7 = 0x0000006f
4: /z $x8 = 0x00000030
(gdb) 
17          addi x6, x6, 1
=> 0x8000001c <acc+4>:  13 03 13 00     addi    t1,t1,1
1: /z $x5 = 0x00000008
2: /z $x6 = 0x80000033
3: /z $x7 = 0x0000006f
4: /z $x8 = 0x00000030
(gdb) 
18          lb x7, 0(x6)
=> 0x80000020 <acc+8>:  83 03 03 00     lb      t2,0(t1)
1: /z $x5 = 0x00000008
2: /z $x6 = 0x80000034
3: /z $x7 = 0x0000006f
4: /z $x8 = 0x00000030
(gdb) 
judge () at test.s:20
20          bne x7, x8, acc
=> 0x80000024 <judge+0>:        e3 9a 83 fe     bne     t2,s0,0x80000018 <acc>
1: /z $x5 = 0x00000008
2: /z $x6 = 0x80000034
3: /z $x7 = 0x00000072
4: /z $x8 = 0x00000030
(gdb) 
acc () at test.s:16
16          addi x5, x5, 1
=> 0x80000018 <acc+0>:  93 82 12 00     addi    t0,t0,1
1: /z $x5 = 0x00000008
2: /z $x6 = 0x80000034
3: /z $x7 = 0x00000072
4: /z $x8 = 0x00000030
(gdb) 
17          addi x6, x6, 1
=> 0x8000001c <acc+4>:  13 03 13 00     addi    t1,t1,1
1: /z $x5 = 0x00000009
2: /z $x6 = 0x80000034
3: /z $x7 = 0x00000072
4: /z $x8 = 0x00000030
(gdb) 
18          lb x7, 0(x6)
=> 0x80000020 <acc+8>:  83 03 03 00     lb      t2,0(t1)
1: /z $x5 = 0x00000009
2: /z $x6 = 0x80000035
3: /z $x7 = 0x00000072
4: /z $x8 = 0x00000030
(gdb) 
judge () at test.s:20
20          bne x7, x8, acc
=> 0x80000024 <judge+0>:        e3 9a 83 fe     bne     t2,s0,0x80000018 <acc>
1: /z $x5 = 0x00000009
2: /z $x6 = 0x80000035
3: /z $x7 = 0x0000006c
4: /z $x8 = 0x00000030
(gdb) 
acc () at test.s:16
16          addi x5, x5, 1
=> 0x80000018 <acc+0>:  93 82 12 00     addi    t0,t0,1
1: /z $x5 = 0x00000009
2: /z $x6 = 0x80000035
3: /z $x7 = 0x0000006c
4: /z $x8 = 0x00000030
(gdb) 
17          addi x6, x6, 1
=> 0x8000001c <acc+4>:  13 03 13 00     addi    t1,t1,1
1: /z $x5 = 0x0000000a
2: /z $x6 = 0x80000035
3: /z $x7 = 0x0000006c
4: /z $x8 = 0x00000030
(gdb) 
18          lb x7, 0(x6)
=> 0x80000020 <acc+8>:  83 03 03 00     lb      t2,0(t1)
1: /z $x5 = 0x0000000a
2: /z $x6 = 0x80000036
3: /z $x7 = 0x0000006c
4: /z $x8 = 0x00000030
(gdb) 
judge () at test.s:20
20          bne x7, x8, acc
=> 0x80000024 <judge+0>:        e3 9a 83 fe     bne     t2,s0,0x80000018 <acc>
1: /z $x5 = 0x0000000a
2: /z $x6 = 0x80000036
3: /z $x7 = 0x00000064
4: /z $x8 = 0x00000030
(gdb) 
acc () at test.s:16
16          addi x5, x5, 1
=> 0x80000018 <acc+0>:  93 82 12 00     addi    t0,t0,1
1: /z $x5 = 0x0000000a
2: /z $x6 = 0x80000036
3: /z $x7 = 0x00000064
4: /z $x8 = 0x00000030
(gdb) 
17          addi x6, x6, 1
=> 0x8000001c <acc+4>:  13 03 13 00     addi    t1,t1,1
1: /z $x5 = 0x0000000b
2: /z $x6 = 0x80000036
3: /z $x7 = 0x00000064
4: /z $x8 = 0x00000030
(gdb) 
18          lb x7, 0(x6)
=> 0x80000020 <acc+8>:  83 03 03 00     lb      t2,0(t1)
1: /z $x5 = 0x0000000b
2: /z $x6 = 0x80000037
3: /z $x7 = 0x00000064
4: /z $x8 = 0x00000030
(gdb) 
judge () at test.s:20
20          bne x7, x8, acc
=> 0x80000024 <judge+0>:        e3 9a 83 fe     bne     t2,s0,0x80000018 <acc>
1: /z $x5 = 0x0000000b
2: /z $x6 = 0x80000037
3: /z $x7 = 0x00000021
4: /z $x8 = 0x00000030
(gdb) 
acc () at test.s:16
16          addi x5, x5, 1
=> 0x80000018 <acc+0>:  93 82 12 00     addi    t0,t0,1
1: /z $x5 = 0x0000000b
2: /z $x6 = 0x80000037
3: /z $x7 = 0x00000021
4: /z $x8 = 0x00000030
(gdb) 
17          addi x6, x6, 1
=> 0x8000001c <acc+4>:  13 03 13 00     addi    t1,t1,1
1: /z $x5 = 0x0000000c
2: /z $x6 = 0x80000037
3: /z $x7 = 0x00000021
4: /z $x8 = 0x00000030
(gdb) 
18          lb x7, 0(x6)
=> 0x80000020 <acc+8>:  83 03 03 00     lb      t2,0(t1)
1: /z $x5 = 0x0000000c
2: /z $x6 = 0x80000038
3: /z $x7 = 0x00000021
4: /z $x8 = 0x00000030
(gdb) 
judge () at test.s:20
20          bne x7, x8, acc
=> 0x80000024 <judge+0>:        e3 9a 83 fe     bne     t2,s0,0x80000018 <acc>
1: /z $x5 = 0x0000000c
2: /z $x6 = 0x80000038
3: /z $x7 = 0x00000030
4: /z $x8 = 0x00000030
(gdb) 
stop () at test.s:23
23          j stop
=> 0x80000028 <stop+0>: 6f 00 00 00     j       0x80000028 <stop>
   0x8000002c <_array+0>:       68 65   flw     fa0,76(a0)
   0x8000002e <_array+2>:       6c 6c   flw     fa1,92(s0)
   0x80000030 <_array+4>:       6f 2c 77 6f     jal     s8,0x80072f26
   0x80000034 <_array+8>:       72 6c   flw     fs8,28(sp)
   0x80000036 <_array+10>:      64 21   fld     fs1,192(a0)
   0x80000038 <_array+12>:      30 00   addi    a2,sp,8
1: /z $x5 = 0x0000000c
2: /z $x6 = 0x80000038
3: /z $x7 = 0x00000030
4: /z $x8 = 0x00000030
(gdb) q
A debugging session is active.

        Inferior 1 [process 1] will be detached.

Quit anyway? (y or n) y
Detaching from program: /home/server/Documents/riscv-operating-system-mooc/code/asm/exer57/test.elf, process 1
Ending remote debugging.
[Inferior 1 (process 1) detached]
```

## 练习5-8

要求：阅读 code/asm/cc_leaf 和 code/asm/cc_nested 的例⼦代码，理解 RISC-V 的函数调⽤约定。在此基 础上编写汇编程序实现以下功能，等价的 C 语⾔的⽰例代码如下，供参考：

```c
unsigned int square(unsigned int i) {
    return i * i; 
} 

unsigned int sum_squares(unsigned int i) {
    unsigned int sum = 0;
    for (int j = 1; j <= i; j++) {
        sum += square(j);
    }
    return sum; 
} 

void _start() {
  sum_squares(3); 
}
```

汇编代码`test.s`如下所示：

```
# C Program:
# unsigned int square(unsigned int i) {
#     return i * i; 
# } 
#
# unsigned int sum_squares(unsigned int i) {
#     unsigned int sum = 0;
#     for (int j = 1; j <= i; j++) {
#         sum += square(j);
#     }
#     return sum; 
# } 
#
# void _start() {
#   sum_squares(3); 
# }

    .text
    .global _start

_start:
    la sp, stack_end
    li a0, 3
    call sum_squares

stop:
    j stop

sum_squares:
    # prologue
    addi sp, sp, -16
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw s2, 8(sp)
    sw ra, 12(sp)
    
    # body
    mv s0, a0       # s0 for i 
    li s1, 0        # s1 for sum
    li s2, 1        # s2 for j
tc:
    bgt s2, s0, return
    mv a0, s2
    call square
    add s1, s1, a0
    addi s2, s2, 1
    j tc
return:
    mv a0, s1

    # epilogue
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw s2, 8(sp)
    lw ra, 12(sp)
    addi sp, sp, 16
    ret

square:
    # prologue
    addi sp, sp, -8
    sw s0, 0(sp)
    sw s1, 4(sp)

    # `mul a0, a0, a0` should be fine,
    # programing as below just to demo we can contine use the stack
    mv s0, a0
    mul s1, s0, s0
    mv a0, s1

    # epilogue
    lw s0, 0(sp)
    lw s1, 4(sp)
    addi sp, sp, 8

    ret

    # add nop here just for demo in gdb
    nop

stack_start:
    .rept 10
    .word 0
    .endr

stack_end:
    .end
```

调试：

```bash
$ make debug
Press Ctrl-C and then input 'quit' to exit GDB and QEMU
-------------------------------------------------------
Reading symbols from test.elf...
Breakpoint 1 at 0x80000000: file test.s, line 22.
0x00001000 in ?? ()
=> 0x00001000:  97 02 00 00     auipc   t0,0x0
1: /z $x1 = 0x00000000
2: /z $x2 = 0x00000000
3: /z $x8 = 0x00000000
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x00000000

Breakpoint 1, _start () at test.s:22
22          la sp, stack_end
=> 0x80000000 <_start+0>:       17 01 00 00     auipc   sp,0x0
   0x80000004 <_start+4>:       13 01 c1 0b     addi    sp,sp,188 # 0x800000bc
1: /z $x1 = 0x00000000
2: /z $x2 = 0x00000000
3: /z $x8 = 0x00000000
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x00000000
(gdb) si
0x80000004 in _start () at test.s:22
22          la sp, stack_end
   0x80000000 <_start+0>:       17 01 00 00     auipc   sp,0x0
=> 0x80000004 <_start+4>:       13 01 c1 0b     addi    sp,sp,188 # 0x800000bc
1: /z $x1 = 0x00000000
2: /z $x2 = 0x80000000
3: /z $x8 = 0x00000000
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x00000000
(gdb) 
_start () at test.s:23
23          li a0, 3
=> 0x80000008 <_start+8>:       13 05 30 00     li      a0,3
1: /z $x1 = 0x00000000
2: /z $x2 = 0x800000bc
3: /z $x8 = 0x00000000
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x00000000
(gdb) 
24          call sum_squares
=> 0x8000000c <_start+12>:      ef 00 80 00     jal     ra,0x80000014 <sum_squares>
1: /z $x1 = 0x00000000
2: /z $x2 = 0x800000bc
3: /z $x8 = 0x00000000
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x00000003
(gdb) 
sum_squares () at test.s:31
31          addi sp, sp, -16
=> 0x80000014 <sum_squares+0>:  13 01 01 ff     addi    sp,sp,-16
1: /z $x1 = 0x80000010
2: /z $x2 = 0x800000bc
3: /z $x8 = 0x00000000
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x00000003
(gdb) 
32          sw s0, 0(sp)
=> 0x80000018 <sum_squares+4>:  23 20 81 00     sw      s0,0(sp)
1: /z $x1 = 0x80000010
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000000
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x00000003
(gdb) 
33          sw s1, 4(sp)
=> 0x8000001c <sum_squares+8>:  23 22 91 00     sw      s1,4(sp)
1: /z $x1 = 0x80000010
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000000
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x00000003
(gdb) 
34          sw s2, 8(sp)
=> 0x80000020 <sum_squares+12>: 23 24 21 01     sw      s2,8(sp)
1: /z $x1 = 0x80000010
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000000
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x00000003
(gdb) 
35          sw ra, 12(sp)
=> 0x80000024 <sum_squares+16>: 23 26 11 00     sw      ra,12(sp)
1: /z $x1 = 0x80000010
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000000
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x00000003
(gdb) 
38          mv s0, a0       # s0 for i 
=> 0x80000028 <sum_squares+20>: 13 04 05 00     mv      s0,a0
1: /z $x1 = 0x80000010
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000000
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x00000003
(gdb) 
39          li s1, 0        # s1 for sum
=> 0x8000002c <sum_squares+24>: 93 04 00 00     li      s1,0
1: /z $x1 = 0x80000010
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x00000003
(gdb) 
40          li s2, 1        # s2 for j
=> 0x80000030 <sum_squares+28>: 13 09 10 00     li      s2,1
1: /z $x1 = 0x80000010
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x00000003
(gdb) 
tc () at test.s:42
42          bgt s2, s0, return
=> 0x80000034 <tc+0>:   63 4c 24 01     blt     s0,s2,0x8000004c <return>
1: /z $x1 = 0x80000010
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000001
6: /z $x10 = 0x00000003
(gdb) 
43          mv a0, s2
=> 0x80000038 <tc+4>:   13 05 09 00     mv      a0,s2
1: /z $x1 = 0x80000010
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000001
6: /z $x10 = 0x00000003
(gdb) 
44          call square
=> 0x8000003c <tc+8>:   ef 00 c0 02     jal     ra,0x80000068 <square>
1: /z $x1 = 0x80000010
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000001
6: /z $x10 = 0x00000001
(gdb) 
square () at test.s:61
61              addi sp, sp, -8
=> 0x80000068 <square+0>:       13 01 81 ff     addi    sp,sp,-8
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000001
6: /z $x10 = 0x00000001
(gdb) 
62              sw s0, 0(sp)
=> 0x8000006c <square+4>:       23 20 81 00     sw      s0,0(sp)
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000001
6: /z $x10 = 0x00000001
(gdb) 
63              sw s1, 4(sp)
=> 0x80000070 <square+8>:       23 22 91 00     sw      s1,4(sp)
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000001
6: /z $x10 = 0x00000001
(gdb) 
67              mv s0, a0
=> 0x80000074 <square+12>:      13 04 05 00     mv      s0,a0
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000001
6: /z $x10 = 0x00000001
(gdb) 
68              mul s1, s0, s0
=> 0x80000078 <square+16>:      b3 04 84 02     mul     s1,s0,s0
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000001
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000001
6: /z $x10 = 0x00000001
(gdb) 
69              mv a0, s1
=> 0x8000007c <square+20>:      13 85 04 00     mv      a0,s1
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000001
4: /z $x9 = 0x00000001
5: /z $x18 = 0x00000001
6: /z $x10 = 0x00000001
(gdb) 
72              lw s0, 0(sp)
=> 0x80000080 <square+24>:      03 24 01 00     lw      s0,0(sp)
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000001
4: /z $x9 = 0x00000001
5: /z $x18 = 0x00000001
6: /z $x10 = 0x00000001
(gdb) 
73              lw s1, 4(sp)
=> 0x80000084 <square+28>:      83 24 41 00     lw      s1,4(sp)
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000001
5: /z $x18 = 0x00000001
6: /z $x10 = 0x00000001
(gdb) 
74              addi sp, sp, 8
=> 0x80000088 <square+32>:      13 01 81 00     addi    sp,sp,8
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000001
6: /z $x10 = 0x00000001
(gdb) 
square () at test.s:76
76              ret
=> 0x8000008c <square+36>:      67 80 00 00     ret
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000001
6: /z $x10 = 0x00000001
(gdb) 
tc () at test.s:45
45          add s1, s1, a0
=> 0x80000040 <tc+12>:  b3 84 a4 00     add     s1,s1,a0
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000001
6: /z $x10 = 0x00000001
(gdb) 
46          addi s2, s2, 1
=> 0x80000044 <tc+16>:  13 09 19 00     addi    s2,s2,1
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000001
5: /z $x18 = 0x00000001
6: /z $x10 = 0x00000001
(gdb) 
47          j tc
=> 0x80000048 <tc+20>:  6f f0 df fe     j       0x80000034 <tc>
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000001
5: /z $x18 = 0x00000002
6: /z $x10 = 0x00000001
(gdb) 
42          bgt s2, s0, return
=> 0x80000034 <tc+0>:   63 4c 24 01     blt     s0,s2,0x8000004c <return>
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000001
5: /z $x18 = 0x00000002
6: /z $x10 = 0x00000001
(gdb) 
43          mv a0, s2
=> 0x80000038 <tc+4>:   13 05 09 00     mv      a0,s2
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000001
5: /z $x18 = 0x00000002
6: /z $x10 = 0x00000001
(gdb) 
44          call square
=> 0x8000003c <tc+8>:   ef 00 c0 02     jal     ra,0x80000068 <square>
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000001
5: /z $x18 = 0x00000002
6: /z $x10 = 0x00000002
(gdb) 
square () at test.s:61
61              addi sp, sp, -8
=> 0x80000068 <square+0>:       13 01 81 ff     addi    sp,sp,-8
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000001
5: /z $x18 = 0x00000002
6: /z $x10 = 0x00000002
(gdb) 
62              sw s0, 0(sp)
=> 0x8000006c <square+4>:       23 20 81 00     sw      s0,0(sp)
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000001
5: /z $x18 = 0x00000002
6: /z $x10 = 0x00000002
(gdb) 
63              sw s1, 4(sp)
=> 0x80000070 <square+8>:       23 22 91 00     sw      s1,4(sp)
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000001
5: /z $x18 = 0x00000002
6: /z $x10 = 0x00000002
(gdb) 
67              mv s0, a0
=> 0x80000074 <square+12>:      13 04 05 00     mv      s0,a0
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000001
5: /z $x18 = 0x00000002
6: /z $x10 = 0x00000002
(gdb) 
68              mul s1, s0, s0
=> 0x80000078 <square+16>:      b3 04 84 02     mul     s1,s0,s0
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000002
4: /z $x9 = 0x00000001
5: /z $x18 = 0x00000002
6: /z $x10 = 0x00000002
(gdb) 
69              mv a0, s1
=> 0x8000007c <square+20>:      13 85 04 00     mv      a0,s1
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000002
4: /z $x9 = 0x00000004
5: /z $x18 = 0x00000002
6: /z $x10 = 0x00000002
(gdb) 
72              lw s0, 0(sp)
=> 0x80000080 <square+24>:      03 24 01 00     lw      s0,0(sp)
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000002
4: /z $x9 = 0x00000004
5: /z $x18 = 0x00000002
6: /z $x10 = 0x00000004
(gdb) 
73              lw s1, 4(sp)
=> 0x80000084 <square+28>:      83 24 41 00     lw      s1,4(sp)
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000004
5: /z $x18 = 0x00000002
6: /z $x10 = 0x00000004
(gdb) 
74              addi sp, sp, 8
=> 0x80000088 <square+32>:      13 01 81 00     addi    sp,sp,8
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000001
5: /z $x18 = 0x00000002
6: /z $x10 = 0x00000004
(gdb) 
square () at test.s:76
76              ret
=> 0x8000008c <square+36>:      67 80 00 00     ret
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000001
5: /z $x18 = 0x00000002
6: /z $x10 = 0x00000004
(gdb) 
tc () at test.s:45
45          add s1, s1, a0
=> 0x80000040 <tc+12>:  b3 84 a4 00     add     s1,s1,a0
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000001
5: /z $x18 = 0x00000002
6: /z $x10 = 0x00000004
(gdb) 
46          addi s2, s2, 1
=> 0x80000044 <tc+16>:  13 09 19 00     addi    s2,s2,1
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000005
5: /z $x18 = 0x00000002
6: /z $x10 = 0x00000004
(gdb) 
47          j tc
=> 0x80000048 <tc+20>:  6f f0 df fe     j       0x80000034 <tc>
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000005
5: /z $x18 = 0x00000003
6: /z $x10 = 0x00000004
(gdb) 
42          bgt s2, s0, return
=> 0x80000034 <tc+0>:   63 4c 24 01     blt     s0,s2,0x8000004c <return>
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000005
5: /z $x18 = 0x00000003
6: /z $x10 = 0x00000004
(gdb) 
43          mv a0, s2
=> 0x80000038 <tc+4>:   13 05 09 00     mv      a0,s2
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000005
5: /z $x18 = 0x00000003
6: /z $x10 = 0x00000004
(gdb) 
44          call square
=> 0x8000003c <tc+8>:   ef 00 c0 02     jal     ra,0x80000068 <square>
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000005
5: /z $x18 = 0x00000003
6: /z $x10 = 0x00000003
(gdb) 
square () at test.s:61
61              addi sp, sp, -8
=> 0x80000068 <square+0>:       13 01 81 ff     addi    sp,sp,-8
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000005
5: /z $x18 = 0x00000003
6: /z $x10 = 0x00000003
(gdb) 
62              sw s0, 0(sp)
=> 0x8000006c <square+4>:       23 20 81 00     sw      s0,0(sp)
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000005
5: /z $x18 = 0x00000003
6: /z $x10 = 0x00000003
(gdb) 
63              sw s1, 4(sp)
=> 0x80000070 <square+8>:       23 22 91 00     sw      s1,4(sp)
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000005
5: /z $x18 = 0x00000003
6: /z $x10 = 0x00000003
(gdb) 
67              mv s0, a0
=> 0x80000074 <square+12>:      13 04 05 00     mv      s0,a0
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000005
5: /z $x18 = 0x00000003
6: /z $x10 = 0x00000003
(gdb) 
68              mul s1, s0, s0
=> 0x80000078 <square+16>:      b3 04 84 02     mul     s1,s0,s0
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000005
5: /z $x18 = 0x00000003
6: /z $x10 = 0x00000003
(gdb) 
69              mv a0, s1
=> 0x8000007c <square+20>:      13 85 04 00     mv      a0,s1
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000009
5: /z $x18 = 0x00000003
6: /z $x10 = 0x00000003
(gdb) 
72              lw s0, 0(sp)
=> 0x80000080 <square+24>:      03 24 01 00     lw      s0,0(sp)
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000009
5: /z $x18 = 0x00000003
6: /z $x10 = 0x00000009
(gdb) 
73              lw s1, 4(sp)
=> 0x80000084 <square+28>:      83 24 41 00     lw      s1,4(sp)
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000009
5: /z $x18 = 0x00000003
6: /z $x10 = 0x00000009
(gdb) 
74              addi sp, sp, 8
=> 0x80000088 <square+32>:      13 01 81 00     addi    sp,sp,8
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000a4
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000005
5: /z $x18 = 0x00000003
6: /z $x10 = 0x00000009
(gdb) 
square () at test.s:76
76              ret
=> 0x8000008c <square+36>:      67 80 00 00     ret
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000005
5: /z $x18 = 0x00000003
6: /z $x10 = 0x00000009
(gdb) 
tc () at test.s:45
45          add s1, s1, a0
=> 0x80000040 <tc+12>:  b3 84 a4 00     add     s1,s1,a0
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x00000005
5: /z $x18 = 0x00000003
6: /z $x10 = 0x00000009
(gdb) 
46          addi s2, s2, 1
=> 0x80000044 <tc+16>:  13 09 19 00     addi    s2,s2,1
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x0000000e
5: /z $x18 = 0x00000003
6: /z $x10 = 0x00000009
(gdb) 
47          j tc
=> 0x80000048 <tc+20>:  6f f0 df fe     j       0x80000034 <tc>
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x0000000e
5: /z $x18 = 0x00000004
6: /z $x10 = 0x00000009
(gdb) 
42          bgt s2, s0, return
=> 0x80000034 <tc+0>:   63 4c 24 01     blt     s0,s2,0x8000004c <return>
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x0000000e
5: /z $x18 = 0x00000004
6: /z $x10 = 0x00000009
(gdb) 
return () at test.s:49
49          mv a0, s1
=> 0x8000004c <return+0>:       13 85 04 00     mv      a0,s1
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x0000000e
5: /z $x18 = 0x00000004
6: /z $x10 = 0x00000009
(gdb) 
52          lw s0, 0(sp)
=> 0x80000050 <return+4>:       03 24 01 00     lw      s0,0(sp)
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000003
4: /z $x9 = 0x0000000e
5: /z $x18 = 0x00000004
6: /z $x10 = 0x0000000e
(gdb) 
53          lw s1, 4(sp)
=> 0x80000054 <return+8>:       83 24 41 00     lw      s1,4(sp)
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000000
4: /z $x9 = 0x0000000e
5: /z $x18 = 0x00000004
6: /z $x10 = 0x0000000e
(gdb) 
54          lw s2, 8(sp)
=> 0x80000058 <return+12>:      03 29 81 00     lw      s2,8(sp)
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000000
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000004
6: /z $x10 = 0x0000000e
(gdb) 
55          lw ra, 12(sp)
=> 0x8000005c <return+16>:      83 20 c1 00     lw      ra,12(sp)
1: /z $x1 = 0x80000040
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000000
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x0000000e
(gdb) 
56          addi sp, sp, 16
=> 0x80000060 <return+20>:      13 01 01 01     addi    sp,sp,16
1: /z $x1 = 0x80000010
2: /z $x2 = 0x800000ac
3: /z $x8 = 0x00000000
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x0000000e
(gdb) 
return () at test.s:57
57          ret
=> 0x80000064 <return+24>:      67 80 00 00     ret
1: /z $x1 = 0x80000010
2: /z $x2 = 0x800000bc
3: /z $x8 = 0x00000000
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x0000000e
(gdb) 
stop () at test.s:27
27          j stop
=> 0x80000010 <stop+0>: 6f 00 00 00     j       0x80000010 <stop>
1: /z $x1 = 0x80000010
2: /z $x2 = 0x800000bc
3: /z $x8 = 0x00000000
4: /z $x9 = 0x00000000
5: /z $x18 = 0x00000000
6: /z $x10 = 0x0000000e
(gdb) qemu-system-riscv32: terminating on signal 2
Quit
(gdb) q
A debugging session is active.

        Inferior 1 [process 1] will be detached.

Quit anyway? (y or n) y
Detaching from program: /home/server/Documents/riscv-operating-system-mooc/code/asm/exer58/test.elf, process 1
Remote communication error.  Target disconnected.: Broken pipe.
```

最后结果a0(x10)为0xe(14)，即为最终结果。

## 练习5-9

在 C 函数中嵌入⼀段汇编，实现 foo 函数中的 `c = a * a + b * b;` 这句 c 语⾔的同等功能。

```c
int foo(int a, int b) {
    int c;
    c = a * a + b * b;
    return c;
}
```

`test.c`如下所示：

```c
int foo(int a, int b) {
    int c;

    asm volatile(
        "mul %[a], %[a], %[a];"
        "mul %[b], %[b], %[b];"
        "add %[c], %[a], %[b];"
        :[c]"=r"(c)
        :[a]"r"(a), [b]"r"(b)
    );

    return c;
}
```

复用`code/asm/c2asm`当中的`test.s`，编译调试：

```bash
$ make debug
Press Ctrl-C and then input 'quit' to exit GDB and QEMU
-------------------------------------------------------
Reading symbols from test.elf...
Breakpoint 1 at 0x80000000: file test.s, line 8.
0x00001000 in ?? ()
=> 0x00001000:  97 02 00 00     auipc   t0,0x0
1: /z $sp = 0x00000000
2: /z $ra = 0x00000000
3: /z $s0 = 0x00000000
4: /z $a0 = 0x00000000
5: /z $a1 = 0x00000000
6: /z $a4 = 0x00000000
7: /z $a5 = 0x00000000

Breakpoint 1, _start () at test.s:8
8               la sp, stack_end        # prepare stack for calling functions
=> 0x80000000 <_start+0>:       17 01 00 00     auipc   sp,0x0
   0x80000004 <_start+4>:       13 01 41 04     addi    sp,sp,68 # 0x80000044 <foo>
1: /z $sp = 0x00000000
2: /z $ra = 0x00000000
3: /z $s0 = 0x00000000
4: /z $a0 = 0x00000000
5: /z $a1 = 0x87000000
6: /z $a4 = 0x00000000
7: /z $a5 = 0x00000000
(gdb) si
0x80000004 in _start () at test.s:8
8               la sp, stack_end        # prepare stack for calling functions
   0x80000000 <_start+0>:       17 01 00 00     auipc   sp,0x0
=> 0x80000004 <_start+4>:       13 01 41 04     addi    sp,sp,68 # 0x80000044 <foo>
1: /z $sp = 0x80000000
2: /z $ra = 0x00000000
3: /z $s0 = 0x00000000
4: /z $a0 = 0x00000000
5: /z $a1 = 0x87000000
6: /z $a4 = 0x00000000
7: /z $a5 = 0x00000000
(gdb) 
_start () at test.s:10
10              li a0, 1
=> 0x80000008 <_start+8>:       13 05 10 00     li      a0,1
1: /z $sp = 0x80000044
2: /z $ra = 0x00000000
3: /z $s0 = 0x00000000
4: /z $a0 = 0x00000000
5: /z $a1 = 0x87000000
6: /z $a4 = 0x00000000
7: /z $a5 = 0x00000000
(gdb) 
11              li a1, 2
=> 0x8000000c <_start+12>:      93 05 20 00     li      a1,2
1: /z $sp = 0x80000044
2: /z $ra = 0x00000000
3: /z $s0 = 0x00000000
4: /z $a0 = 0x00000001
5: /z $a1 = 0x87000000
6: /z $a4 = 0x00000000
7: /z $a5 = 0x00000000
(gdb) 
12              call foo
=> 0x80000010 <_start+16>:      ef 00 40 03     jal     ra,0x80000044 <foo>
1: /z $sp = 0x80000044
2: /z $ra = 0x00000000
3: /z $s0 = 0x00000000
4: /z $a0 = 0x00000001
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000000
7: /z $a5 = 0x00000000
(gdb) 
0x80000044 in foo ()
=> 0x80000044 <foo+0>:  13 01 01 fd     addi    sp,sp,-48
1: /z $sp = 0x80000044
2: /z $ra = 0x80000014
3: /z $s0 = 0x00000000
4: /z $a0 = 0x00000001
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000000
7: /z $a5 = 0x00000000
(gdb) 
0x80000048 in foo (a=0, b=0) at test.c:1
1       int foo(int a, int b) {
   0x80000044 <foo+0>:  13 01 01 fd     addi    sp,sp,-48
=> 0x80000048 <foo+4>:  23 26 81 02     sw      s0,44(sp)
   0x8000004c <foo+8>:  13 04 01 03     addi    s0,sp,48
   0x80000050 <foo+12>: 23 2e a4 fc     sw      a0,-36(s0)
   0x80000054 <foo+16>: 23 2c b4 fc     sw      a1,-40(s0)
1: /z $sp = 0x80000014
2: /z $ra = 0x80000014
3: /z $s0 = 0x00000000
4: /z $a0 = 0x00000001
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000000
7: /z $a5 = 0x00000000
(gdb) 
0x8000004c      1       int foo(int a, int b) {
   0x80000044 <foo+0>:  13 01 01 fd     addi    sp,sp,-48
   0x80000048 <foo+4>:  23 26 81 02     sw      s0,44(sp)
=> 0x8000004c <foo+8>:  13 04 01 03     addi    s0,sp,48
   0x80000050 <foo+12>: 23 2e a4 fc     sw      a0,-36(s0)
   0x80000054 <foo+16>: 23 2c b4 fc     sw      a1,-40(s0)
1: /z $sp = 0x80000014
2: /z $ra = 0x80000014
3: /z $s0 = 0x00000000
4: /z $a0 = 0x00000001
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000000
7: /z $a5 = 0x00000000
(gdb) 
0x80000050      1       int foo(int a, int b) {
   0x80000044 <foo+0>:  13 01 01 fd     addi    sp,sp,-48
   0x80000048 <foo+4>:  23 26 81 02     sw      s0,44(sp)
   0x8000004c <foo+8>:  13 04 01 03     addi    s0,sp,48
=> 0x80000050 <foo+12>: 23 2e a4 fc     sw      a0,-36(s0)
   0x80000054 <foo+16>: 23 2c b4 fc     sw      a1,-40(s0)
1: /z $sp = 0x80000014
2: /z $ra = 0x80000014
3: /z $s0 = 0x80000044
4: /z $a0 = 0x00000001
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000000
7: /z $a5 = 0x00000000
(gdb) 
0x80000054      1       int foo(int a, int b) {
   0x80000044 <foo+0>:  13 01 01 fd     addi    sp,sp,-48
   0x80000048 <foo+4>:  23 26 81 02     sw      s0,44(sp)
   0x8000004c <foo+8>:  13 04 01 03     addi    s0,sp,48
   0x80000050 <foo+12>: 23 2e a4 fc     sw      a0,-36(s0)
=> 0x80000054 <foo+16>: 23 2c b4 fc     sw      a1,-40(s0)
1: /z $sp = 0x80000014
2: /z $ra = 0x80000014
3: /z $s0 = 0x80000044
4: /z $a0 = 0x00000001
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000000
7: /z $a5 = 0x00000000
(gdb) 
4           asm volatile(
=> 0x80000058 <foo+20>: 83 27 c4 fd     lw      a5,-36(s0)
   0x8000005c <foo+24>: 03 27 84 fd     lw      a4,-40(s0)
   0x80000060 <foo+28>: b3 87 f7 02     mul     a5,a5,a5
   0x80000064 <foo+32>: 33 07 e7 02     mul     a4,a4,a4
   0x80000068 <foo+36>: b3 87 e7 00     add     a5,a5,a4
   0x8000006c <foo+40>: 23 26 f4 fe     sw      a5,-20(s0)
1: /z $sp = 0x80000014
2: /z $ra = 0x80000014
3: /z $s0 = 0x80000044
4: /z $a0 = 0x00000001
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000000
7: /z $a5 = 0x00000000
(gdb) 
0x8000005c      4           asm volatile(
   0x80000058 <foo+20>: 83 27 c4 fd     lw      a5,-36(s0)
=> 0x8000005c <foo+24>: 03 27 84 fd     lw      a4,-40(s0)
   0x80000060 <foo+28>: b3 87 f7 02     mul     a5,a5,a5
   0x80000064 <foo+32>: 33 07 e7 02     mul     a4,a4,a4
   0x80000068 <foo+36>: b3 87 e7 00     add     a5,a5,a4
   0x8000006c <foo+40>: 23 26 f4 fe     sw      a5,-20(s0)
1: /z $sp = 0x80000014
2: /z $ra = 0x80000014
3: /z $s0 = 0x80000044
4: /z $a0 = 0x00000001
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000000
7: /z $a5 = 0x00000001
(gdb) 
0x80000060      4           asm volatile(
   0x80000058 <foo+20>: 83 27 c4 fd     lw      a5,-36(s0)
   0x8000005c <foo+24>: 03 27 84 fd     lw      a4,-40(s0)
=> 0x80000060 <foo+28>: b3 87 f7 02     mul     a5,a5,a5
   0x80000064 <foo+32>: 33 07 e7 02     mul     a4,a4,a4
   0x80000068 <foo+36>: b3 87 e7 00     add     a5,a5,a4
   0x8000006c <foo+40>: 23 26 f4 fe     sw      a5,-20(s0)
1: /z $sp = 0x80000014
2: /z $ra = 0x80000014
3: /z $s0 = 0x80000044
4: /z $a0 = 0x00000001
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000002
7: /z $a5 = 0x00000001
(gdb) 
0x80000064      4           asm volatile(
   0x80000058 <foo+20>: 83 27 c4 fd     lw      a5,-36(s0)
   0x8000005c <foo+24>: 03 27 84 fd     lw      a4,-40(s0)
   0x80000060 <foo+28>: b3 87 f7 02     mul     a5,a5,a5
=> 0x80000064 <foo+32>: 33 07 e7 02     mul     a4,a4,a4
   0x80000068 <foo+36>: b3 87 e7 00     add     a5,a5,a4
   0x8000006c <foo+40>: 23 26 f4 fe     sw      a5,-20(s0)
1: /z $sp = 0x80000014
2: /z $ra = 0x80000014
3: /z $s0 = 0x80000044
4: /z $a0 = 0x00000001
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000002
7: /z $a5 = 0x00000001
(gdb) 
0x80000068      4           asm volatile(
   0x80000058 <foo+20>: 83 27 c4 fd     lw      a5,-36(s0)
   0x8000005c <foo+24>: 03 27 84 fd     lw      a4,-40(s0)
   0x80000060 <foo+28>: b3 87 f7 02     mul     a5,a5,a5
   0x80000064 <foo+32>: 33 07 e7 02     mul     a4,a4,a4
=> 0x80000068 <foo+36>: b3 87 e7 00     add     a5,a5,a4
   0x8000006c <foo+40>: 23 26 f4 fe     sw      a5,-20(s0)
1: /z $sp = 0x80000014
2: /z $ra = 0x80000014
3: /z $s0 = 0x80000044
4: /z $a0 = 0x00000001
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000004
7: /z $a5 = 0x00000001
(gdb) 
0x8000006c      4           asm volatile(
   0x80000058 <foo+20>: 83 27 c4 fd     lw      a5,-36(s0)
   0x8000005c <foo+24>: 03 27 84 fd     lw      a4,-40(s0)
   0x80000060 <foo+28>: b3 87 f7 02     mul     a5,a5,a5
   0x80000064 <foo+32>: 33 07 e7 02     mul     a4,a4,a4
   0x80000068 <foo+36>: b3 87 e7 00     add     a5,a5,a4
=> 0x8000006c <foo+40>: 23 26 f4 fe     sw      a5,-20(s0)
1: /z $sp = 0x80000014
2: /z $ra = 0x80000014
3: /z $s0 = 0x80000044
4: /z $a0 = 0x00000001
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000004
7: /z $a5 = 0x00000005
(gdb) 
12          return c;
=> 0x80000070 <foo+44>: 83 27 c4 fe     lw      a5,-20(s0)
1: /z $sp = 0x80000014
2: /z $ra = 0x80000014
3: /z $s0 = 0x80000044
4: /z $a0 = 0x00000001
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000004
7: /z $a5 = 0x00000005
(gdb) 
13      }
=> 0x80000074 <foo+48>: 13 85 07 00     mv      a0,a5
   0x80000078 <foo+52>: 03 24 c1 02     lw      s0,44(sp)
   0x8000007c <foo+56>: 13 01 01 03     addi    sp,sp,48
   0x80000080 <foo+60>: 67 80 00 00     ret
1: /z $sp = 0x80000014
2: /z $ra = 0x80000014
3: /z $s0 = 0x80000044
4: /z $a0 = 0x00000001
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000004
7: /z $a5 = 0x00000005
(gdb) 
0x80000078      13      }
   0x80000074 <foo+48>: 13 85 07 00     mv      a0,a5
=> 0x80000078 <foo+52>: 03 24 c1 02     lw      s0,44(sp)
   0x8000007c <foo+56>: 13 01 01 03     addi    sp,sp,48
   0x80000080 <foo+60>: 67 80 00 00     ret
1: /z $sp = 0x80000014
2: /z $ra = 0x80000014
3: /z $s0 = 0x80000044
4: /z $a0 = 0x00000005
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000004
7: /z $a5 = 0x00000005
(gdb) 
0x8000007c      13      }
   0x80000074 <foo+48>: 13 85 07 00     mv      a0,a5
   0x80000078 <foo+52>: 03 24 c1 02     lw      s0,44(sp)
=> 0x8000007c <foo+56>: 13 01 01 03     addi    sp,sp,48
   0x80000080 <foo+60>: 67 80 00 00     ret
1: /z $sp = 0x80000014
2: /z $ra = 0x80000014
3: /z $s0 = 0x00000000
4: /z $a0 = 0x00000005
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000004
7: /z $a5 = 0x00000005
(gdb) 
0x80000080      13      }
   0x80000074 <foo+48>: 13 85 07 00     mv      a0,a5
   0x80000078 <foo+52>: 03 24 c1 02     lw      s0,44(sp)
   0x8000007c <foo+56>: 13 01 01 03     addi    sp,sp,48
=> 0x80000080 <foo+60>: 67 80 00 00     ret
1: /z $sp = 0x80000044
2: /z $ra = 0x80000014
3: /z $s0 = 0x00000000
4: /z $a0 = 0x00000005
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000004
7: /z $a5 = 0x00000005
(gdb) 
stop () at test.s:15
15              j stop                  # Infinite loop to stop execution
=> 0x80000014 <stop+0>: 6f 00 00 00     j       0x80000014 <stop>
1: /z $sp = 0x80000044
2: /z $ra = 0x80000014
3: /z $s0 = 0x00000000
4: /z $a0 = 0x00000005
5: /z $a1 = 0x00000002
6: /z $a4 = 0x00000004
7: /z $a5 = 0x00000005
(gdb) qemu-system-riscv32: terminating on signal 2
Quit
(gdb) quit
A debugging session is active.

        Inferior 1 [process 1] will be detached.

Quit anyway? (y or n) y
Detaching from program: /home/server/Documents/riscv-operating-system-mooc/code/asm/exer59/test.elf, process 1
Remote communication error.  Target disconnected.: Broken pipe.
```
# 练习3-1

`hello.c`源程序文件如下：

```c
#include <stdio.h>

int main(){
	printf("hello world.\n");
	return 0;
}
```

对源文件进行本地编译，生成针对支持x86_64指令集架构处理器的目标文件`hello.o`：

```bash
gcc -c hello.c -o hello.o
```

查看`hello.o`的文件的文件头信息：

```bash
$ readelf -h hello.o
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              REL (Relocatable file)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x0
  Start of program headers:          0 (bytes into file)
  Start of section headers:          792 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           0 (bytes)
  Number of program headers:         0
  Size of section headers:           64 (bytes)
  Number of section headers:         14
  Section header string table index: 13
```

查看`hello.o`的section header table：

```bash
$ readelf -SW hello.o
There are 14 section headers, starting at offset 0x318:

Section Headers:
  [Nr] Name              Type            Address          Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            0000000000000000 000000 000000 00      0   0  0
  [ 1] .text             PROGBITS        0000000000000000 000040 00001b 00  AX  0   0  1
  [ 2] .rela.text        RELA            0000000000000000 000258 000030 18   I 11   1  8
  [ 3] .data             PROGBITS        0000000000000000 00005b 000000 00  WA  0   0  1
  [ 4] .bss              NOBITS          0000000000000000 00005b 000000 00  WA  0   0  1
  [ 5] .rodata           PROGBITS        0000000000000000 00005b 00000d 00   A  0   0  1
  [ 6] .comment          PROGBITS        0000000000000000 000068 00002a 01  MS  0   0  1
  [ 7] .note.GNU-stack   PROGBITS        0000000000000000 000092 000000 00      0   0  1
  [ 8] .note.gnu.property NOTE            0000000000000000 000098 000020 00   A  0   0  8
  [ 9] .eh_frame         PROGBITS        0000000000000000 0000b8 000038 00   A  0   0  8
  [10] .rela.eh_frame    RELA            0000000000000000 000288 000018 18   I 11   9  8
  [11] .symtab           SYMTAB          0000000000000000 0000f0 000138 18     12  10  8
  [12] .strtab           STRTAB          0000000000000000 000228 000029 00      0   0  1
  [13] .shstrtab         STRTAB          0000000000000000 0002a0 000074 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  l (large), p (processor specific)
```

对`hello.o`反汇编，并查看`hello.c`的C程序源码和机器指令的对应关系（要包含源代码则需要在编译时加上`-g`选项）：

```bash
$ objdump -S hello.o

hello.o:     file format elf64-x86-64

Disassembly of section .text:

0000000000000000 <main>:
#include <stdio.h>

int main(){
   0:	f3 0f 1e fa          	endbr64 
   4:	55                   	push   %rbp
   5:	48 89 e5             	mov    %rsp,%rbp
	printf("hello world.\n");
   8:	48 8d 3d 00 00 00 00 	lea    0x0(%rip),%rdi        # f <main+0xf>
   f:	e8 00 00 00 00       	callq  14 <main+0x14>
	return 0;
  14:	b8 00 00 00 00       	mov    $0x0,%eax
}
  19:	5d                   	pop    %rbp
  1a:	c3                   	retq
```

# 练习3-2

如下例⼦ C 语⾔代码：

```bash
#include <stdio.h> 

int global_init = 0x11111111; 
const int global_const = 0x22222222; 

void main() {
		static int static_var = 0x33333333; 
		static int static_var_uninit; 

		int auto_var = 0x44444444; 

		printf("hello world!\n"); 
		return;
}
```

保存为`test.c`，编译后，使用`objdump`的`-s`、`-x`以及`-d`选项：

```bash
-x, --all-headers        Display the contents of all headers
-s, --full-contents      Display the full contents of all sections requested
-d, --disassemble        Display assembler contents of executable sections
```

得到：

```bash
$ objdump -x -s -d -W test.o

test.o:     file format elf64-x86-64
test.o
architecture: i386:x86-64, flags 0x00000011:
HAS_RELOC, HAS_SYMS
start address 0x0000000000000000

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         00000022  0000000000000000  0000000000000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  1 .data         00000008  0000000000000000  0000000000000000  00000064  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000004  0000000000000000  0000000000000000  0000006c  2**2
                  ALLOC
  3 .rodata       00000011  0000000000000000  0000000000000000  0000006c  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  4 .comment      0000002a  0000000000000000  0000000000000000  0000007d  2**0
                  CONTENTS, READONLY
  5 .note.GNU-stack 00000000  0000000000000000  0000000000000000  000000a7  2**0
                  CONTENTS, READONLY
  6 .note.gnu.property 00000020  0000000000000000  0000000000000000  000000a8  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  7 .eh_frame     00000038  0000000000000000  0000000000000000  000000c8  2**3
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
SYMBOL TABLE:
0000000000000000 l    df *ABS*	0000000000000000 test.c
0000000000000000 l    d  .text	0000000000000000 .text
0000000000000000 l    d  .data	0000000000000000 .data
0000000000000000 l    d  .bss	0000000000000000 .bss
0000000000000000 l    d  .rodata	0000000000000000 .rodata
0000000000000000 l     O .bss	0000000000000004 static_var_uninit.2318
0000000000000004 l     O .data	0000000000000004 static_var.2317
0000000000000000 l    d  .note.GNU-stack	0000000000000000 .note.GNU-stack
0000000000000000 l    d  .note.gnu.property	0000000000000000 .note.gnu.property
0000000000000000 l    d  .eh_frame	0000000000000000 .eh_frame
0000000000000000 l    d  .comment	0000000000000000 .comment
0000000000000000 g     O .data	0000000000000004 global_init
0000000000000000 g     O .rodata	0000000000000004 global_const
0000000000000000 g     F .text	0000000000000022 main
0000000000000000         *UND*	0000000000000000 _GLOBAL_OFFSET_TABLE_
0000000000000000         *UND*	0000000000000000 puts

Contents of the .eh_frame section:

00000000 0000000000000014 00000000 CIE
  Version:               1
  Augmentation:          "zR"
  Code alignment factor: 1
  Data alignment factor: -8
  Return address column: 16
  Augmentation data:     1b
  DW_CFA_def_cfa: r7 (rsp) ofs 8
  DW_CFA_offset: r16 (rip) at cfa-8
  DW_CFA_nop
  DW_CFA_nop

00000018 000000000000001c 0000001c FDE cie=00000000 pc=0000000000000000..0000000000000022
  DW_CFA_advance_loc: 5 to 0000000000000005
  DW_CFA_def_cfa_offset: 16
  DW_CFA_offset: r6 (rbp) at cfa-16
  DW_CFA_advance_loc: 3 to 0000000000000008
  DW_CFA_def_cfa_register: r6 (rbp)
  DW_CFA_advance_loc: 25 to 0000000000000021
  DW_CFA_def_cfa: r7 (rsp) ofs 8
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop

Contents of section .text:
 0000 f30f1efa 554889e5 4883ec10 c745fc44  ....UH..H....E.D
 0010 44444448 8d3d0000 0000e800 00000090  DDDH.=..........
 0020 c9c3                                 ..              
Contents of section .data:
 0000 11111111 33333333                    ....3333        
Contents of section .rodata:
 0000 22222222 68656c6c 6f20776f 726c6421  """"hello world!
 0010 00                                   .               
Contents of section .comment:
 0000 00474343 3a202855 62756e74 7520392e  .GCC: (Ubuntu 9.
 0010 342e302d 31756275 6e747531 7e32302e  4.0-1ubuntu1~20.
 0020 30342920 392e342e 3000               04) 9.4.0.      
Contents of section .note.gnu.property:
 0000 04000000 10000000 05000000 474e5500  ............GNU.
 0010 020000c0 04000000 03000000 00000000  ................
Contents of section .eh_frame:
 0000 14000000 00000000 017a5200 01781001  .........zR..x..
 0010 1b0c0708 90010000 1c000000 1c000000  ................
 0020 00000000 22000000 00450e10 8602430d  ...."....E....C.
 0030 06590c07 08000000                    .Y......        

Disassembly of section .text:

0000000000000000 <main>:
   0:	f3 0f 1e fa          	endbr64 
   4:	55                   	push   %rbp
   5:	48 89 e5             	mov    %rsp,%rbp
   8:	48 83 ec 10          	sub    $0x10,%rsp
   c:	c7 45 fc 44 44 44 44 	movl   $0x44444444,-0x4(%rbp)
  13:	48 8d 3d 00 00 00 00 	lea    0x0(%rip),%rdi        # 1a <main+0x1a>
			16: R_X86_64_PC32	.rodata
  1a:	e8 00 00 00 00       	callq  1f <main+0x1f>
			1b: R_X86_64_PLT32	puts-0x4
  1f:	90                   	nop
  20:	c9                   	leaveq 
  21:	c3                   	retq
```

可以得到：

- `global_init`位于`.data`
- `global_const`位于`.rodata`
- `static_var`位于`.data`
- `static_var_uninit`位于`.bss`
- `auto_var`位于`.text`
- `"hello world!\n"`字符串位于`.rodata`
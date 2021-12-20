# 练习

## 练习7-1

要求：完全采⽤汇编语⾔重写 code/os/01-helloRVOS，运⾏后在控制台上打印输出 "Hello, RVOS!”

汇编代码如下start.S，直接在asm文件夹下的环境来运行：

```
# size of each hart's stack is 1024 bytes
	.equ	STACK_SIZE, 1024
    .equ    MAXNUM_CPU, 8
    .equ    UART0, 0x10000000
    .equ    RHR, 0	# Receive Holding Register (read mode)
    .equ    THR, 0	# Transmit Holding Register (write mode)
    .equ    DLL, 0	# LSB of Divisor Latch (write mode)
    .equ    IER, 1	# Interrupt Enable Register (write mode)
    .equ    DLM, 1	# MSB of Divisor Latch (write mode)
    .equ    FCR, 2	# FIFO Control Register (write mode)
    .equ    ISR, 2	# Interrupt Status Register (read mode)
    .equ    LCR, 3	# Line Control Register
    .equ    MCR, 4	# Modem Control Register
    .equ    LSR, 5	# Line Status Register
    .equ    MSR, 6	# Modem Status Register
    .equ    SPR, 7	# ScratchPad Register
    .equ    LSR_RX_READY, 1
    .equ    LSR_TX_IDLE, 32

	.global	_start

	.text
_start:
	# park harts with id != 0
	csrr	t0, mhartid		# read current hart id
	mv	tp, t0			# keep CPU's hartid in its tp for later usage.
	bnez	t0, park		# if we're not on the hart 0
					# we park the hart
	# Setup stacks, the stack grows from bottom to top, so we put the
	# stack pointer to the very end of the stack range.
	slli	t0, t0, 10		# shift left the hart id by 1024
	la	sp, stacks + STACK_SIZE	# set the initial stack pointer
					# to the end of the first stack space
	add	sp, sp, t0		# move the current hart stack pointer
					# to its place in the stack space

	j	start_kernel		# hart 0 jump to c

start_kernel:
    j   uart_init

uart_init:
    li  s0, UART0       # UART0 base addr
    li  t1, 0
    sb  t1, IER(s0)     # uart_write_reg(IER, 0x00);
    lb  t1, LCR(s0)     # uint8_t lcr = uart_read_reg(LCR);
    ori t1, t1, 128     # uart_write_reg(LCR, lcr | (1 << 7));
    sb  t1, LCR(s0)
    li  t2, 3
    sb  t2, DLL(s0)     # uart_write_reg(DLL, 0x03);
    li  t2, 0
    sb  t2, DLM(s0)     # uart_write_reg(DLM, 0x00);
    ori t2, t2, 3
    sb  t2, LCR(s0)     # uart_write_reg(LCR, lcr | (3 << 0));
    j   uart_puts

uart_puts:
    la t0, _array
    lb t1, 0(t0)
    li t2, '\0'
    j judge
acc:
    lb t3, LSR(s0)
    and t3, t3, LSR_TX_IDLE
    beqz t3, acc
    sb t1, THR(s0)
    addi t0, t0, 1
    lb t1, 0(t0)
judge:
    bne t1, t2, acc

stop:
    j stop

park:
	wfi
	j	park

_array:
    .byte 'H'
    .byte 'e'
    .byte 'l'
    .byte 'l'
    .byte 'o'
    .byte ','
    .byte ' '
    .byte 'R'
    .byte 'V'
    .byte 'O'
    .byte 'S'
    .byte '!'
    .byte '\0'

stacks:
	.skip	STACK_SIZE * MAXNUM_CPU # allocate space for all the harts stacks

	.end				# End of file
```

运行输出：

```bash
$ make run
Press Ctrl-A and then X to exit QEMU
------------------------------------
No output, please run 'make debug' to see details
Hello, RVOS!
```

为了简便起见，没有使用函数调用的方式来处理。

## 练习7-2

这道问题没有实际做出来，因为还不清楚如何使用qemu控制台来进行互动，但思路大概就是类似`uart_putc`的方式来处理，先判断LSR寄存器来判断是否接收到了数据，然后读取`RHR`，之后将RHR的值直接放入`uart_putc`当中即可。
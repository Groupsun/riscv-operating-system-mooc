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

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

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

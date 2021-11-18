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
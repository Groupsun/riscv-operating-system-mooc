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
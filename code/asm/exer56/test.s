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

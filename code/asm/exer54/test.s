
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
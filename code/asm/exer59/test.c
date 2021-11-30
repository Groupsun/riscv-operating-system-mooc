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
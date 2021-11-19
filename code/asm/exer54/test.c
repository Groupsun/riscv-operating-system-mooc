#include <stdio.h>

int main() {
    int a = 0x87654321;
    unsigned int low = a & 0x0000ffff;
    unsigned int high = a & 0xffff0000;
    high = high >> 16;

    printf("low = 0x%x, high = 0x%x\n", low, high);
    return 0;
}
#include "global.h"

void printBlock64(char * str, Block64 b)
{
    printf("%s %02x %02x %02x %02x %02x %02x %02x %02x\n", str, b.b8.w0, b.b8.w1, b.b8.w2, b.b8.w3, b.b8.w4, b.b8.w5, b.b8.w6, b.b8.w7);
}

void printBlock32(char * str, Block32 b)
{
    printf("%s %02x %02x %02x %02x\n", str, b.b8.w0, b.b8.w1, b.b8.w2, b.b8.w3);
}

void printBlock16(char * str, Block16 b)
{
    printf("%s %02x %02x\n", str, b.b8.w0, b.b8.w1);
}

void swapBytes(Block64 * b)
{
    uint8_t swap = b->b8.w0;
    b->b8.w0 = b->b8.w7;
    b->b8.w7 = swap;

    swap = b->b8.w1;
    b->b8.w1 = b->b8.w6;
    b->b8.w6 = swap;

    swap = b->b8.w2;
    b->b8.w2 = b->b8.w5;
    b->b8.w5 = swap;

    swap = b->b8.w3;
    b->b8.w3 = b->b8.w4;
    b->b8.w4 = swap;
}
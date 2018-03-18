#ifndef BLOCK_HEADER
#define BLOCK_HEADER

typedef union Block16 {
    struct {
        uint8_t w1;
        uint8_t w0;
    } b8;
    struct {
        uint16_t w0;
    } b16;
} Block16;

typedef union Block32 {
    struct {
        uint8_t w3;
        uint8_t w2;
        uint8_t w1;
        uint8_t w0;
    } b8;
    struct {
        uint16_t w1;
        uint16_t w0;
    } b16;
    struct {
        uint32_t w0;
    } b32;
} Block32;

typedef union Block64 {
    struct {
        uint8_t w7;
        uint8_t w6;
        uint8_t w5;
        uint8_t w4;
        uint8_t w3;
        uint8_t w2;
        uint8_t w1;
        uint8_t w0;
    } b8;
    struct {
        uint16_t w3;
        uint16_t w2;
        uint16_t w1;
        uint16_t w0;
    } b16;
    struct {
        uint32_t w1;
        uint32_t w0;
    } b32;
    struct {
        uint64_t w0;
    } b64;
} Block64;

void printBlock64(char * str, Block64 b);
void printBlock32(char * str, Block32 b);
void printBlock16(char * str, Block16 b);
void swapBytes(Block64 * b);

#endif
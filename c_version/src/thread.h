#ifndef THREAD_HEADER
#define THREAD_HEADER

#include "global.h"

void *worker(void * id);

void run(Block64 * block);

// F block
Block32 F(uint32_t w, uint8_t * r, uint8_t * i);

// G block
Block16 G(uint8_t g1, uint8_t g2, uint8_t * r, uint8_t * i);

// get value from f table
uint8_t F_Table(uint8_t x);

#endif
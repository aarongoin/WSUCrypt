
#include "global.h"

void *worker(void * id)
{
    uint16_t i; // index of block

    while (shouldLive)
    {
        // assign self the next available block
        pthread_mutex_lock(&mutex);
        // wait until block is available
        if (nextIndex == length)
        {
            pthread_mutex_unlock(&mutex);
            continue;
        }

        i = nextIndex++;
        pthread_mutex_unlock(&mutex);

        if (shouldLive == 0)
            return 0;

        if (DEBUG)
            printf("Thread %d working on block %d\n", *((uint8_t *)id), i);

        // do operation on block
        run(&blocks[i]);

        pthread_mutex_lock(&mutex);
        done++;
        pthread_mutex_unlock(&mutex);
    }
    free(id);

    return 0;
}

void run(Block64 * block)
{
    uint8_t key_row = isEncrypt ? 0 : 15; // index of subkey row
    uint8_t key_col = 0; // index of subkey column

    // whiten input
    block->b64.w0 ^= key.b64.w0;

    if (DEBUG)
        printBlock64("Whitened block:", *block);

    // run 16 rounds
    uint8_t r;
    for (r = 0; r < 16; r++)
    {
        Block32 swap;
        swap.b32.w0 = block->b32.w0;
        //r0 = r2 ^ f0
        //r1 = r3 ^ f1
        block->b32.w0 = block->b32.w1 ^ F(block->b32.w0, &key_row, &key_col).b32.w0;
        //r2 = orig_r0
        //r3 = orig_r1
        block->b32.w1 = swap.b32.w0;

        if (DEBUG)
            printBlock64("Block:", *block);
    }

    // undo last swap
    Block32 swap;
    swap.b32.w0 = block->b32.w0;
    block->b32.w0 = block->b32.w1;
    block->b32.w1 = swap.b32.w0;

    // whiten output
    block->b64.w0 ^= key.b64.w0;

    if (isEncrypt == 0)
        swapBytes(block);

    if (DEBUG)
        printBlock64("Output block:", *block);
}

// F block
Block32 F(uint32_t w, uint8_t * r, uint8_t * i)
{
    Block32 word;
    word.b32.w0 = w;
    Block16 t0 = G(word.b8.w0, word.b8.w1, r, i);
    Block16 t1 = G(word.b8.w2, word.b8.w3, r, i);

    if (DEBUG)
    {
        printBlock16("t0:", t0);
        printBlock16("t1:", t1);
    }

    Block32 f;
    Block16 sk;

    sk.b8.w0 = subkey(r, i);
    sk.b8.w1 = subkey(r, i);
    f.b16.w0 = (t0.b16.w0 + 2 * t1.b16.w0 + sk.b16.w0) % 65536;

    sk.b8.w0 = subkey(r, i);
    sk.b8.w1 = subkey(r, i);
    f.b16.w1 = (2 * t0.b16.w0 + t1.b16.w0 + sk.b16.w0) % 65536;

    if (DEBUG)
        printBlock32("F:", f);

    return f;
}

// G block
Block16 G(uint8_t g1, uint8_t g2, uint8_t * r, uint8_t * i)
{
    uint8_t g3 = F_Table(g2 ^ subkey(r, i)) ^ g1;
    uint8_t g4 = F_Table(g3 ^ subkey(r, i)) ^ g2;
    uint8_t g5 = F_Table(g4 ^ subkey(r, i)) ^ g3;
    uint8_t g6 = F_Table(g5 ^ subkey(r, i)) ^ g4;

    if (DEBUG)
        printf("g1: %02x, g2: %02x g3: %02x, g4: %02x ,g5: %02x, g6: %02x\n", g1, g2, g3, g4, g5, g6);

    Block16 g;
    g.b8.w0 = g5;
    g.b8.w1 = g6;

    return g;
}

// get value from f table
uint8_t F_Table(uint8_t x)
{
    uint16_t row = (x >> 4);
    uint16_t col = x & 0xf;

    // printf("F_table row: %01x, col: %01x\n", row, col);
    return ftable[row * 16 + col];
}
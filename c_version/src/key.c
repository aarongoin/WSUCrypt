#include "global.h"

void getKey()
{
    // generate key if encrypting, else read it from file
    if (isEncrypt)
    {
        generateKey();
        fwrite(&key, sizeof(uint64_t), 1, key_file);
    }
    else if (fread(&key, sizeof(uint64_t), 1, key_file) == 0)
    {
        exit(1);
    }
    if (DEBUG)
        printBlock64("Key:", key);
}

// generate random key
void generateKey()
{
    key.b64.w0 = 0xabcdef0123456789;
}

// generate subkeys from current key value
void generateSubkeys()
{
    if (DEBUG)
        printf("Subkeys:\n");
    
    uint8_t r;
    for (r = 0; r < 16; r++)
    {
        uint8_t index = 0;
        uint8_t z;
        uint8_t i;
        for (z = 0; z < 3; z++)
            for (i = 0; i < 4; i++)
            {
                subkeys[r][index] = K(4 * r + i);
                if (DEBUG)
                    printf("%02x ", subkeys[r][index]);
                index++;
            }
        if (DEBUG)
            printf("\n");
    }
    if (DEBUG)
        printf("\n");
}

// subkey creation function
uint8_t K(uint8_t x)
{
    LCR(&key.b64.w0); // left circular shift by one bit
    switch (x % 8)
    {
    case 0:
        return key.b8.w7;
    case 1:
        return key.b8.w6;
    case 2:
        return key.b8.w5;
    case 3:
        return key.b8.w4;
    case 4:
        return key.b8.w3;
    case 5:
        return key.b8.w2;
    case 6:
        return key.b8.w1;
    case 7:
        return key.b8.w0;
    }
    return 0;
}

// left circular rotate one bit
void LCR(uint64_t * bits)
{
    *bits = (*bits << 1) | (*bits >> 63);
}

// subkey iterator function
uint8_t subkey(uint8_t * r, uint8_t * i)
{
    if (*i == 12)
    {
        *i = 0;
        if (isEncrypt)
            (*r)++;
        else
            (*r)--;
    }
    //printf("Subkey at (%d, %d): %02x\n", *r, *i, subkeys[*r][*i]);
    return subkeys[(*r)][(*i)++];
}
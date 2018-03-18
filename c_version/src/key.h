#ifndef KEY_HEADER
#define KEY_HEADER

void getKey();

// generate random key
void generateKey();

// generate subkeys from current key value
void generateSubkeys();

// subkey creation function
uint8_t K(uint8_t x);

// left circular rotate one bit
void LCR(uint64_t * bits);

// subkey iterator function
uint8_t subkey(uint8_t * r, uint8_t * i);

#endif
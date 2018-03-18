# WSUCrypt

By: Aaron Goin

WSUCrypt is a 64bit block encryption algorithm that is an mashup of the Twofish and SKIPJACK encryption aglorithms.

It is implemented and working correctly in C. The C version uses pthreads for blazing fast encryption and decryption.

I also have code for a half-implemented JavaScript version, but have not yet had the time to finish it.

**NOTE: This encryption algorithm is for educational purposes, and is not recommended for secure use.**

## Build and Use

Building the C version requires GCC and Make be installed on your system.

1. Navigate to `/WSUCrypt/c_version/` in your shell terminal and enter `make` to compile WSUCrypt on your system.

2. To encrypt a file, name it `plaintext.txt` and move it into the same directory as the WSUCrypt executable.

3. Execute `./WSUCrypt -e` to encrypt `plaintext.txt` to `ciphertext.txt` using the first 64 bytes of `key.txt` as the key.

4. Execute `./WSUCrypt -d` to decrypt `ciphertext.txt` to `plaintext.txt` using the first 64 bytes of `key.txt` as the key.
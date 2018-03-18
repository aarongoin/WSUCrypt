// WSUCrypt
// by: Aaron Goin
//
// Encrypt and decrypt files
//
// usage:
//      WSUCrypt -e, --encrypt [file] [...options]      encrypt file or ./plaintext.txt
//      WSUCrypt -d, --decrypt [file] [...options]      decrypt file or ./ciphertext.txt
//      WSUCrypt -h, --help                             display usage instructions
// options:
//      -i, --input                 read input from STDIN
//      -o, --output [file]         output result to file or to STDOUT if no file given
//      -k, --key file              read key from file (cannot use -p option)
//      -p, --password password     use password as key (cannot use -k)
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "global.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
///////////////////////////////////////////////////////////////////////////////////////////////////

void openFiles();
void getKey();
void generateKey();
void generateSubkeys();
void startThreads();
void readInBlocks();
void cleanup(int signal);

///////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
///////////////////////////////////////////////////////////////////////////////////////////////////

char *input_path;
char *output_path;
char *key_path;

FILE *input;
FILE *output;
FILE *key_file;

uint8_t cpus = 0;
pthread_t *threads;

uint8_t inputFromStdin = 0;
uint8_t outputToStdout = 0;
uint8_t useKeyFile = 1;
uint8_t isEncrypt = 0;
uint8_t shouldLive = 1; // used so threads know when to stop themselves

Block64 blocks[BUFFER_SIZE];                       // blocks buffer
uint16_t length = 0;                               // number of blocks in the buffer
uint16_t nextIndex = 0;                            // next available index for worker to take
uint16_t done = 0;                                 // count of completed blocks
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // used when modifying index or done

Block64 key;
uint8_t subkeys[16][12];

int chunks_handled = 0;

// skipjack f-table
uint8_t ftable[] = {
    0xa3, 0xd7, 0x09, 0x83, 0xf8, 0x48, 0xf6, 0xf4, 0xb3, 0x21, 0x15, 0x78, 0x99, 0xb1, 0xaf, 0xf9,
    0xe7, 0x2d, 0x4d, 0x8a, 0xce, 0x4c, 0xca, 0x2e, 0x52, 0x95, 0xd9, 0x1e, 0x4e, 0x38, 0x44, 0x28,
    0x0a, 0xdf, 0x02, 0xa0, 0x17, 0xf1, 0x60, 0x68, 0x12, 0xb7, 0x7a, 0xc3, 0xe9, 0xfa, 0x3d, 0x53,
    0x96, 0x84, 0x6b, 0xba, 0xf2, 0x63, 0x9a, 0x19, 0x7c, 0xae, 0xe5, 0xf5, 0xf7, 0x16, 0x6a, 0xa2,
    0x39, 0xb6, 0x7b, 0x0f, 0xc1, 0x93, 0x81, 0x1b, 0xee, 0xb4, 0x1a, 0xea, 0xd0, 0x91, 0x2f, 0xb8,
    0x55, 0xb9, 0xda, 0x85, 0x3f, 0x41, 0xbf, 0xe0, 0x5a, 0x58, 0x80, 0x5f, 0x66, 0x0b, 0xd8, 0x90,
    0x35, 0xd5, 0xc0, 0xa7, 0x33, 0x06, 0x65, 0x69, 0x45, 0x00, 0x94, 0x56, 0x6d, 0x98, 0x9b, 0x76,
    0x97, 0xfc, 0xb2, 0xc2, 0xb0, 0xfe, 0xdb, 0x20, 0xe1, 0xeb, 0xd6, 0xe4, 0xdd, 0x47, 0x4a, 0x1d,
    0x42, 0xed, 0x9e, 0x6e, 0x49, 0x3c, 0xcd, 0x43, 0x27, 0xd2, 0x07, 0xd4, 0xde, 0xc7, 0x67, 0x18,
    0x89, 0xcb, 0x30, 0x1f, 0x8d, 0xc6, 0x8f, 0xaa, 0xc8, 0x74, 0xdc, 0xc9, 0x5d, 0x5c, 0x31, 0xa4,
    0x70, 0x88, 0x61, 0x2c, 0x9f, 0x0d, 0x2b, 0x87, 0x50, 0x82, 0x54, 0x64, 0x26, 0x7d, 0x03, 0x40,
    0x34, 0x4b, 0x1c, 0x73, 0xd1, 0xc4, 0xfd, 0x3b, 0xcc, 0xfb, 0x7f, 0xab, 0xe6, 0x3e, 0x5b, 0xa5,
    0xad, 0x04, 0x23, 0x9c, 0x14, 0x51, 0x22, 0xf0, 0x29, 0x79, 0x71, 0x7e, 0xff, 0x8c, 0x0e, 0xe2,
    0x0c, 0xef, 0xbc, 0x72, 0x75, 0x6f, 0x37, 0xa1, 0xec, 0xd3, 0x8e, 0x62, 0x8b, 0x86, 0x10, 0xe8,
    0x08, 0x77, 0x11, 0xbe, 0x92, 0x4f, 0x24, 0xc5, 0x32, 0x36, 0x9d, 0xcf, 0xf3, 0xa6, 0xbb, 0xac,
    0x5e, 0x6c, 0xa9, 0x13, 0x57, 0x25, 0xb5, 0xe3, 0xbd, 0xa8, 0x3a, 0x01, 0x05, 0x59, 0x2a, 0x46};

///////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN PROCESS FUNCTION
///////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{

    clock_t start = 0;

    signal(SIGINT | SIGKILL | SIGTERM, &cleanup);

    // handle arguments

    if (argc == 2)
    {
        isEncrypt = (strcmp(argv[1], "-e") == 0) ? 1 : 0;

        if (isEncrypt)
        {
            input_path = "./plaintext.txt";
            output_path = "./ciphertext.txt";
        }
        else
        {
            input_path = "./ciphertext.txt";
            output_path = "./plaintext.txt";
        }

        if (useKeyFile)
            key_path = "./key.txt";
    }

    if (DEBUG)
        printf("\n%s\n", isEncrypt ? "Encrypting..." : "Decrypting...");

    if (BENCHMARK)
        start = clock();

    openFiles();
    getKey();
    generateSubkeys();
    startThreads();
    readInBlocks();

    if (BENCHMARK)
        printf("\nExecuted in %f milliseconds.\n\n", (clock() - start) / 1000.0f);

    cleanup(0);

    return 0;
}

void openFiles()
{
    // open input and output files
    input = fopen(input_path, "rb");
    output = fopen(output_path, "wb");

    assert(input != NULL);
    assert(output != NULL);

    if (useKeyFile)
    {
        key_file = fopen(key_path, "wb");
        assert(key_file != NULL);
    }
}

void startThreads()
{
    // dividing by two in case of hyperthreading masking the true number of cores
    cpus = sysconf(_SC_NPROCESSORS_CONF) / 2;

    if (cpus < 1)
        cpus = 1;

    if (DEBUG)
        printf("Creating %d threads\n", cpus);

    threads = calloc(cpus, sizeof(pthread_t));
    uint8_t i;
    for (i = 0; i < cpus; i++)
    {
        uint8_t *id = malloc(sizeof(uint8_t));
        *id = i;
        if (pthread_create(&threads[i], NULL, worker, id) != 0)
        {
            free(id);
            cleanup(0);
            exit(1);
        }
        // else printf("Created thread %d\n", i);
    }
}

void readInBlocks()
{
    Block64 block;
    block.b64.w0 = 0;
    uint8_t qty = 8; // number of bytes read into block (8 to make a full block)
    uint64_t buf = 0;

    if (DEBUG)
        printf("Reading input file...\n");

    // read blocks from file until EOF
    while ((qty = fread(&buf, 1, 8, input)))
    {

        block.b64.w0 = buf;
        buf = 0;
        // bytes written big-endian
        if (isEncrypt)
            swapBytes(&block);

        if (DEBUG)
            printBlock64("Read bytes:", block);

        // if buffer is full then we need to write it to disk
        if (length == BUFFER_SIZE)
        {
            // wait until entire block has been processed by workers
            while (done != length)
                usleep(250);
            // write the buffer to output file
            if (DEBUG)
                printf("Writing block buffer %d to output\n", chunks_handled++);
            fwrite(blocks, sizeof(uint64_t), BUFFER_SIZE, output);
            // reset our buffer indices
            pthread_mutex_lock(&mutex);
            length = 0;
            nextIndex = 0;
            done = 0;
            pthread_mutex_unlock(&mutex);
        }
        // write block into buffer
        blocks[length] = block;
        block.b64.w0 = 0;
        length++;
        if (DEBUG)
            printf("Added block #%d\n", length);
    }
    if (DEBUG)
        printf("Done reading\n");

    // write buffer to output file
    if (length)
    {
        while (done != length)
            usleep(250);
            
        if (DEBUG)
            printf("Writing block buffer to output\n");
        fwrite(blocks, sizeof(uint64_t), length, output);
    }
}

void cleanup(int signal)
{
    shouldLive = 0;
    // close file handlers
    if (input != NULL)
        fclose(input);
    if (output != NULL)
        fclose(output);
    if (key_file != NULL)
        fclose(key_file);

    // clean up our threads
    uint8_t i;
    for (i = 0; i < cpus; i++)
        pthread_join(threads[i], NULL);

    printf("\nGoodbye!\n\n");

    // free memory
    free(threads);
}

void printHelp()
{
    printf("usage:\n");
    printf("\tWSUCrypt -e [file] [...options]     encrypt file or ./plaintext.txt\n");
    printf("\tWSUCrypt -d [file] [...options]     decrypt file or ./ciphertext.txt\n");
    printf("\tWSUCrypt -h                         display usage instructions\n");
    // printf("options:\n");
    // printf("\t-i              read input from STDIN\n");
    // printf("\t-o [file]       output result to file or to STDOUT if no file given\n");
    // printf("\t-k file         read key from file (cannot use -p option)\n");
    // printf("\t-p password     use password as key (cannot use -k)\n");
}
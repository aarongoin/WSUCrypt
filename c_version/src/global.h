#ifndef GLOBALS
#define GLOBALS


///////////////////////////////////////////////////////////////////////////////////////////////////
// IMPORT ALL THE THINGS!
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "block.h"
#include "thread.h"
#include "key.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
// MACROS & SUCH
///////////////////////////////////////////////////////////////////////////////////////////////////

#define BUFFER_SIZE 16384
#define DEBUG 0
#define BENCHMARK 1


///////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBALS from: main.c
///////////////////////////////////////////////////////////////////////////////////////////////////

extern FILE *key_file;

extern uint8_t isEncrypt;
extern uint8_t shouldLive;    // used so threads know when to stop themselves

extern Block64 blocks[BUFFER_SIZE];       // blocks buffer
extern uint16_t length;       // number of blocks in the buffer
extern uint16_t nextIndex;    // next available index for worker to take
extern uint16_t done;         // count of completed blocks
extern pthread_mutex_t mutex; // used when modifying index or done

extern Block64 key;
extern uint8_t subkeys[16][12];

// skipjack f-table
extern uint8_t ftable[256];

#endif
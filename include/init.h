#ifndef __INIT_H
#define __INIT_H

#include "ijvm.h"

// Data blocks and program counter. Inıtialized at init_ijvm() function inside machine.c code.
struct Blocks {
    word_t program_counter;  // Program counter. This must be ijvm instance specific.

    int const_size;
    word_t *constants;

    int text_size;
    byte_t *texts;
};

// output file and input file of the ijvm.
FILE *outfile, *infile;

// Swaps integer from big-endian to little endian.
static uint32_t swap_uint32(uint32_t num) {
    return ((num >> 24) & 0xff) | ((num << 8) & 0xff0000) | ((num >> 8) & 0xff00) | ((num << 24) & 0xff000000);
}

// Initialize data blocks.
struct Blocks blocks;

#endif

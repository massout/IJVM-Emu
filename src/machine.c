#include <stdlib.h>

#include "ijvm.h"
#include "init.h"
#include "ins.h"
#include "stack.h"

// This function initializes an ijvm instance from given file.
int init_ijvm(char *binary_file) {
    FILE *fp;        // File pointer that represents given file.
    word_t wbuffer;  // Word sized buffer for integer reading.

    blocks.program_counter = 0;  // Initialization of program counter to zero.

    // Initialize each infile and outfile to temporary file.
    outfile = tmpfile();
    infile = tmpfile();

    if ((fp = fopen(binary_file, "rb")) != NULL) {  // If file in the argument is can be opened (or just not NULL).
        fread(&wbuffer, sizeof(wbuffer), 1, fp);    // Read the magic number.

        if (swap_uint32(wbuffer) == MAGIC_NUMBER) {   // If magic number is correct just pass.
            fread(&wbuffer, sizeof(wbuffer), 1, fp);  // Dummy reading. Not for use just for continuing in the file.

            fread(&wbuffer, sizeof(wbuffer), 1, fp);                                  // Read the RAW number of constants.
            blocks.const_size = (int)swap_uint32(wbuffer) / sizeof(byte_t);           // This calculates REAL number of constants with swapping litte-big endian and take division to size of one element.
            blocks.constants = (word_t *)malloc(sizeof(word_t) * blocks.const_size);  // This allocates memory for constants with given number/size of them.
            fread(blocks.constants, sizeof(byte_t), blocks.const_size, fp);           // This reads constants from file as many as given number and prints to allocated fields.

            fread(&wbuffer, sizeof(wbuffer), 1, fp);  // Dummy reading. Not for use just for continuing in the file.

            fread(&wbuffer, sizeof(wbuffer), 1, fp);                             // Read the RAW number of texts.
            blocks.text_size = (int)swap_uint32(wbuffer);                        // This is the REAL number of texts. Because each text is byte sized.
            blocks.texts = (byte_t *)malloc(sizeof(byte_t) * blocks.text_size);  // This allocates memory for constants with given number/size of them.
            fread(blocks.texts, sizeof(byte_t), blocks.text_size, fp);           // This reads text from file as many as given number and prints to allocated fields.

            initStack();  // Calls stack initialization function.

            return 0;  // If ijvm instance succesfully created this function will return zero.
        }
    }
    return -1;  // If there is a problem this function will return -1.
}

void destroy_ijvm(void) {
    free(st);
}

// This function takes the text field as pointer and iterates inside of it.
// With switch-case control structure this function will call specific function for index program counter in text.
// If index program counter in text equals to OP_HALT or OP_ERR it will returns false as a value. Otherwise it will returns true value.
bool step(void) {
    byte_t *text = get_text();

    if (text[get_program_counter()] == OP_HALT || text[get_program_counter()] == OP_ERR) {
        return false;
    } else {
        switch (text[get_program_counter()]) {
            case OP_BIPUSH:
                bipush();
                break;

            case OP_IADD:
                iadd();
                break;

            case OP_DUP:
                dup();
                break;

            case OP_IAND:
                iand();
                break;

            case OP_IOR:
                ior();
                break;

            case OP_ISUB:
                isub();
                break;

            case OP_SWAP:
                swap();
                break;

            case OP_POP:
                pop();
                break;

            case OP_OUT:
                out();
                break;

            case OP_GOTO:
                go2();
                break;

            case OP_NOP:
                nop();
                break;

            case OP_ILOAD:
                iload();
                break;

            case OP_ISTORE:
                istore();
                break;

            case OP_LDC_W:
                ldcw();
                break;

            case OP_IFEQ:
                ifeq();
                break;

            case OP_IFLT:
                iflt();
                break;

            case OP_ICMPEQ:
                icmpeq();
                break;

            case OP_IINC:
                iinc();
                break;

            case OP_INVOKEVIRTUAL:
                invokevirtual();
                break;

            case OP_IRETURN:
                ireturn();
                break;

            case OP_IN:
                in();
                break;
        }
        return true;
    }
}

// Returns text field of ijvm instance.
byte_t *get_text(void) {
    return blocks.texts;
}

// Return program counter of ijvm instance.
int get_program_counter(void) {
    return blocks.program_counter;
}

// Return instruction of program counter index in text field.
byte_t get_instruction(void) {
    return blocks.texts[blocks.program_counter];
}

// Return size of stack.
int stack_size(void) {
    return st->size;
}

// Return size of text field.
int text_size(void) {
    return blocks.text_size;
}

// Set output file of ijvm instance.
void set_output(FILE *f) {
    outfile = f;
}

// Return top of stack with peeking.
word_t tos(void) {
    fprintf(stderr, "tos(): %d %x\n", stackPeek(), stackPeek());
    return stackPeek();
}

// Return the main stack.
word_t *get_stack(void) {
    return st->global_vec;
}

// Execute given program with multiple stepping.
void run(void) {
    for (int i = 0; blocks.program_counter >= 0 && blocks.program_counter < blocks.text_size; i++) {
        if (!step())
            break;
    }
}

// Get local variable in given index.
word_t get_local_variable(int i) {
    if (st->lv == 0) {
        fprintf(stderr, "glv(): %d %x\n", st->lvars[i], st->lvars[i]);
        return st->lvars[i];
    } else {
        fprintf(stderr, "glv(): %d %x\n", st->global_vec[(st->lv + 1) + i], st->global_vec[(st->lv + 1) + i]);
        return st->global_vec[(st->lv + 1) + i];
    }
}

// Get constant variable in given index in constant pool.
word_t get_constant(int i) {
    return blocks.constants[i];
}

// Set input file of ijvm instance.
void set_input(FILE *f) {
    infile = f;
}

bool finished() {
    if (get_text()[get_program_counter()] == OP_HALT && get_text()[get_program_counter()] == OP_ERR)
        return true;
    else
        return false;
}

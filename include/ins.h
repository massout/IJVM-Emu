#ifndef __INS_H
#define __INS_H

#include "init.h"
#include "stack.h"

// Operation takes one byte as argument and pushes to stack. Finally increases program counter 2 which that skips to next instruction.
void bipush() {
    int8_t data = blocks.texts[blocks.program_counter + 1];
    stackPush(data);
    blocks.program_counter += 2;
}

// Operation peeks (just taking top of stack and not pop) and pushes peeked data to stack. Finally increases program counter 2 which that skips to next instruction.
void dup() {
    word_t data = stackPeek();
    stackPush(data);
    blocks.program_counter++;
}

// This operation takes 2 bytes (short) as signed and add-equal to program counter. Later program counter skips to label [index of text] given.
void go2() {
    blocks.program_counter += (int16_t)((blocks.texts[blocks.program_counter + 1] << 8) + blocks.texts[blocks.program_counter + 2]);
}

// Takes 2 Pop from stack, adds and pushes the sum of 2 pops.
void iadd() {
    stackPush(stackPop() + stackPop());
    blocks.program_counter++;
}

// Takes 2 pop form stack, pushes this 2 pop's bitwise-and equal.
void iand() {
    stackPush(stackPop() & stackPop());
    blocks.program_counter++;
}

// Takes 2 pop form stack, pushes this 2 pop's bitwise-or equal.
void ior() {
    stackPush(stackPop() | stackPop());
    blocks.program_counter++;
}

// If data popped from stack is equal zero, program counter is add-equal goes with given 16bit signed to label. Else goes to next instruction.
void ifeq() {
    if (!stackPop())
        blocks.program_counter += (int16_t)((blocks.texts[blocks.program_counter + 1] << 8) + blocks.texts[blocks.program_counter + 2]);
    else
        blocks.program_counter += 3;
}

// If data popped from stack is lesser than zero, program counter is add-equal goes with given 16bit signed to label. Else goes to next instruction.
void iflt() {
    if (stackPop() < 0)
        blocks.program_counter += (int16_t)((blocks.texts[blocks.program_counter + 1] << 8) + blocks.texts[blocks.program_counter + 2]);
    else
        blocks.program_counter += 3;
}

// If two data popped from stack is equal, program counter is add-equal goes with given 16bit signed to label. Else goes to next instruction.
void icmpeq() {
    if (stackPop() == stackPop())
        blocks.program_counter += (int16_t)((blocks.texts[blocks.program_counter + 1] << 8) + blocks.texts[blocks.program_counter + 2]);
    else
        blocks.program_counter += 3;
}

// Takes two arguments; first is the byte of the local variable's index, second is the byte value we want to add to local variable at the first argument.
void iinc() {
    if (st->lv == 0)
        st->lvars[blocks.texts[blocks.program_counter + 1]] += (int8_t)(blocks.texts[blocks.program_counter + 2]);
    else
        st->global_vec[(word_t)(st->lv + 1) + blocks.texts[blocks.program_counter + 1]] += (int8_t)(blocks.texts[blocks.program_counter + 2]);

    blocks.program_counter += 3;
}

// Takes two data from stack and pushes the value of the second - first and pushes value of this.
void isub() {
    word_t num1 = stackPop();
    word_t num2 = stackPop();
    stackPush(num2 - num1);
    blocks.program_counter++;
}

// Takes two data from stack and pushes first taken data and secen taken data with given sequence. This operation swaps places of top two data of stack.
void swap() {
    word_t buffer = stackPop();
    word_t buffer1 = stackPop();
    stackPush(buffer);
    stackPush(buffer1);

    blocks.program_counter++;
}

// Prints char data of popped value from the stack to -outfile- file pointer.
void out() {
    fprintf(outfile, "%c", (char)stackPop());
    blocks.program_counter++;
}

// Pops data from stack.
void pop() {
    stackPop();
    blocks.program_counter++;
}

// Does nothing
void nop() {
    blocks.program_counter++;
}

// Loads data from local variable array to stack.
void iload() {
    if (st->lv == 0)
        stackPush(st->lvars[(uint32_t)(blocks.texts[blocks.program_counter + 1])]);
    else
        stackPush(st->global_vec[(uint32_t)((st->lv + 1) + blocks.texts[blocks.program_counter + 1])]);

    blocks.program_counter += 2;
}

// Stores data from popped value of stack inside of local variable array in given index.
void istore() {
    if (st->lv == 0)
        st->lvars[(uint32_t)(blocks.texts[blocks.program_counter + 1])] = stackPop();
    else
        st->global_vec[(uint32_t)((st->lv + 1) + (blocks.texts[blocks.program_counter + 1]))] = stackPop();

    blocks.program_counter += 2;
}

// Loads wanted index at data in constants pool to stack.
void ldcw() {
    stackPush(swap_uint32(blocks.constants[((blocks.texts[blocks.program_counter + 1] << 8) + blocks.texts[blocks.program_counter + 2])]));
    blocks.program_counter += 3;
}

// Takes a char from selected -infile- and pushes to stack.
void in() {
    word_t data = getc(infile);
    if (data != EOF)
        stackPush(data);
    else
        stackPush(0);

    blocks.program_counter++;
}

void invokevirtual() {
    word_t old_lv = st->lv;
    word_t old_pc = blocks.program_counter;

    // Equals (goes to) program counter with function adress. Takes number of parameters and number of variables.
    blocks.program_counter = swap_uint32(blocks.constants[(uint16_t)((blocks.texts[blocks.program_counter + 1] << 8) + blocks.texts[blocks.program_counter + 2])]);

    short num_of_params = (short)((blocks.texts[blocks.program_counter] << 8) + blocks.texts[blocks.program_counter + 1]);
    uint16_t num_of_vars = ((blocks.texts[blocks.program_counter + 2] << 8) + blocks.texts[blocks.program_counter + 3]);
    num_of_vars = abs(num_of_vars);

    // Creates a buffer that contains parameters and pops from stack and sequences in reverse order.
    word_t* params_buffer = (word_t*)malloc(sizeof(word_t) * num_of_params);

    for (int i = num_of_params - 1; i >= 0; i--)
        params_buffer[i] = stackPop();

    // Equals 1 addition of stack pointer to local variable pointer.
    st->lv = ++st->sp;

    // Pushes buffer of parameters to stack.
    for (int i = 0; i < num_of_params; i++)
        stackPush(params_buffer[i]);

    // Throwing garbage.
    free(params_buffer);
    params_buffer = NULL;

    // Adding number of variables to stack pointer. This makes stack pointer increase number of vars.
    st->sp += num_of_vars;

    // If stack pointer is larger than size of stack it resizes stack with count of number of variables.
    if (st->sp > st->size)
        stackResize(st->size + 2 * num_of_vars);

    // Pushes old program counter to stack.
    stackPush(old_pc);

    // Equals stack pointer to local variable pointer index of stack.
    st->global_vec[st->lv] = st->sp;

    // Pushes old local variable pointer.
    stackPush(old_lv);

    blocks.program_counter += 4;
}

// Quit from current method and return(push) last word to stack.
void ireturn() {
    // Link pointer is the index of local vector in main vector of stack.
    word_t link_ptr = st->global_vec[st->lv];

    // This is our lastest word value. Poppes from current stack.
    word_t return_val = stackPop();

    // Equals program counter to index of link pointer in main stack vector and its 3 addition.
    blocks.program_counter = st->global_vec[link_ptr] + 3;

    // Equals stack pointer to local variable pointer.
    st->sp = st->lv;
    // Equals local variable pointer to index of (link pointer + 1) in main stack vector.
    st->lv = st->global_vec[link_ptr + 1];

    // Preperation for returning.
    st->sp--;

    // Return the value to stack.
    stackPush(return_val);
}

#endif

#ifndef __STACK_H
#define __STACK_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "ijvm.h"

struct Stack {
    word_t sp;  // The stack pointer.
    word_t lv;  // Local variable pointer.

    word_t *lvars;  // Local variable vector.

    word_t *global_vec;  // Main stack vector.
    word_t size;         // Size of stack.
} * st;

// Initializes stack.
void initStack() {
    // Memory allocation for stack pointer.
    st = (struct Stack *)malloc(sizeof(struct Stack));

    // Size of stack equals that multiply of size of total text and size of a word.
    st->size = text_size() * sizeof(word_t);

    // Memory allocation for local variable vector pointer.
    st->lvars = (word_t *)malloc(sizeof(word_t) * 65536);
    // Memory allocation for main stack pointer.
    st->global_vec = (word_t *)malloc(sizeof(word_t) * st->size);

    // Index initialization.
    st->lv = 0;
    st->sp = 0;
}

// Popping operation from top of stack.
word_t stackPop() {
    if (st->sp < 0) return NULL;

    return st->global_vec[st->sp--];
}

// Peeking the top of stack. This operation not deletes top of stack.
word_t stackPeek() {
    if (st->sp < 0) return NULL;

    return st->global_vec[st->sp];
}

// Returns size of (1 addition of stack pointer) stack.
word_t stackSize() {
    return st->sp + 1;
}

// Resizes stack with given argument. And changes stack's size variable.
void stackResize(word_t new_size) {
    st->global_vec = (word_t *)realloc(st->global_vec, new_size * sizeof(word_t));
    st->size = new_size;
}

// Pushes a data to top of stack. Also this function controls size and if size is lower than stack pointer's 2 substract it calls resize function.
void stackPush(word_t data) {
    if (st->sp == st->size - 2)
        stackResize(st->size * 2);

    st->global_vec[++st->sp] = data;
}

#endif

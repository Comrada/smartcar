/*  
 *          File: stack.h
 *        Author: John Bode (http://stackoverflow.com/users/134554/john-bode)
 *         Topic: Stack - Array Implementation
 * ----------------------------------------------------------------
 *
 * This is the interface for a stack of characters.
 */

#ifndef _STACK_H
#define _STACK_H

#include <stdio.h>
#include <stdlib.h>  /* for dynamic allocation */

/**
 * Type for individual stack entry
 */
struct stack_entry {
  char *data;
  struct stack_entry *next;
};

/**
 * Type for stack instance
 */
struct stack_t
{
  struct stack_entry *head;
  size_t stackSize;  // not strictly necessary, but
                     // useful for logging
};

struct stack_t *newStack(void);
char *copyString(char *str);
void push(struct stack_t *theStack, char *value);
char *top(struct stack_t *theStack);
void pop(struct stack_t *theStack);
void clear(struct stack_t *theStack);
void destroyStack(struct stack_t **theStack);

#endif  /* not defined _STACK_H */

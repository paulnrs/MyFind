#ifndef STACK_H
#define STACK_H

#include <stddef.h>

struct stack
{
    size_t size;
    struct token *token;
    struct stack *next;
};

struct stack *stack_init(void);
struct stack *stack_push(struct stack *s, struct token *tok);
struct stack *stack_pop(struct stack *s);
struct token *stack_peek(struct stack *s);

#endif /* !STACK_H */

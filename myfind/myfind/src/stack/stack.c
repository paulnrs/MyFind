#include "stack.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct stack *stack_init(void)
{
    struct stack *new = malloc(sizeof(struct stack));
    new->size = 0;
    return new;
}

struct stack *stack_push(struct stack *s, struct token *tok)
{
    if (!s)
    {
        struct stack *new = malloc(sizeof(struct stack));
        new->token = tok;
        new->next = NULL;
        new->size = 1;
        return new;
    }
    struct stack *new = malloc(sizeof(struct stack));
    new->token = tok;
    new->next = s;
    new->size = s->size + 1;
    return new;
}

struct stack *stack_pop(struct stack *s)
{
    if (s == NULL)
    {
        return NULL;
    }
    else
    {
        struct stack *tmp = s->next;
        s->size -= 1;
        free(s);
        return tmp;
    }
}

struct token *stack_peek(struct stack *s)
{
    return s->token;
}

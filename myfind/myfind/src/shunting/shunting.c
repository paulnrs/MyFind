#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/parser.h"
#include "../stack/stack.h"

static struct token *init2(char *data, enum type t, int prio, enum asso a)
{
    struct token *new = malloc(sizeof(struct token));
    new->data = data;
    new->type = t;
    new->prio = prio;
    new->asso = a;
    new->left = NULL;
    new->right = NULL;
    return new;
}

static int is_operand2(struct token *token)
{
    if (token->type == EXPR_PRINT)
    {
        return 1;
    }
    if (token->type == EXPR_NAME)
    {
        return 1;
    }
    if (token->type == EXPR_TYPE)
    {
        return 1;
    }
    if (token->type == EXPR_NEWER)
    {
        return 1;
    }
    if (token->type == EXPR_PERM)
    {
        return 1;
    }
    if (token->type == EXPR_USER)
    {
        return 1;
    }
    if (token->type == EXPR_GROUP)
    {
        return 1;
    }
    return 0;
}

static void handle(struct stack **operator, struct stack ** operands)
{
    struct token *peek = stack_peek(*operator);
    *operator= stack_pop(*operator);
    if (peek->type == NOT)
    {
        peek->right = stack_peek(*operands);
        *operands = stack_pop(*operands);
    }
    else
    {
        peek->right = stack_peek(*operands);
        *operands = stack_pop(*operands);
        peek->left = stack_peek(*operands);
        *operands = stack_pop(*operands);
    }
    *operands = stack_push(*operands, peek);
}

static void aux_right(struct stack **operator, struct stack ** operands)
{
    while (stack_peek(*operator)->type != LEFT)
    {
        handle(&(*operator), &(*operands));
    }
    if (stack_peek(*operator)->type != LEFT)
    {
        err(2, "Lexical Error");
    }
    else
    {
        *operator= stack_pop(*operator);
    }
}
static int is_good(struct token ***token, struct stack **operator, int tok)
{
    if ((*operator)->size > 0 && stack_peek(*operator)->asso != N
        && stack_peek(*operator)->type != LEFT
        && ((((*token)[tok]->asso == L)
             && ((*token)[tok]->prio <= stack_peek(*operator)->prio))
            || (((*token)[tok]->asso == R)
                && ((*token)[tok]->prio < stack_peek(*operator)->prio))))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static void handle_no(struct stack **operands)
{
    struct token *and = init2(NULL, AND, 3, L);
    and->right = init2(NULL, EXPR_PRINT, 1, L);
    if ((*operands)->size == 0)
    {
        and->left = NULL;
    }
    else
    {
        and->left = stack_peek(*operands);
        *operands = stack_pop(*operands);
    }
    *operands = stack_push(*operands, and);
}

struct stack *shunting(struct token **token, int tok, int prev_operand,
                       int *no_action)
{
    struct stack *operator= stack_init();
    struct stack *operands = stack_init();
    while (token[tok])
    {
        if (is_operand2(token[tok]))
        {
            if (prev_operand == 0)
            {
                struct token *and = init2(NULL, AND, 3, L);
                operator= stack_push(operator, and);
            }
            operands = stack_push(operands, token[tok++]);
            prev_operand = 0;
        }
        else if (token[tok]->asso != N)
        {
            while (is_good(&token, &operator, tok))
            {
                handle(&operator, & operands);
            }
            operator= stack_push(operator, token[tok++]);
            prev_operand = 1;
        }
        else if (token[tok]->type == LEFT)
        {
            operator= stack_push(operator, token[tok++]);
            prev_operand = 1;
        }
        else if (token[tok]->type == RIGHT)
        {
            aux_right(&operator, & operands);
            tok++;
        }
    }
    while (operator->size> 0 && stack_peek(operator))
    {
        if (stack_peek(operator)->type == LEFT
            || stack_peek(operator)->asso == N)
        {
            err(2, "Lexical Error");
        }
        else
        {
            handle(&operator, & operands);
        }
    }
    if (*no_action == 0)
    {
        /*if (is_not_start(&token))
            operands = stack_push(operands, init2(NULL, EXPR_PRINT, 1, L));
            else*/
        handle_no(&operands);
    }
    free(operator);
    return operands;
}

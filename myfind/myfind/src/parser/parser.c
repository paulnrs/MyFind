#include "parser.h"

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_token(struct token **list)
{
    for (int i = 0; list[i] != NULL; i++)
    {
        free(list[i]);
    }
    free(list);
}

static struct token *init(char *data, enum type t, int prio, enum asso a)
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

static int is_operand(char *operand)
{
    if (strcmp(operand, "-name") == 0)
    {
        return 1;
    }
    if (strcmp(operand, "-type") == 0)
    {
        return 1;
    }
    if (strcmp(operand, "-newer") == 0)
    {
        return 1;
    }
    if (strcmp(operand, "-perm") == 0)
    {
        return 1;
    }
    if (strcmp(operand, "-user") == 0)
    {
        return 1;
    }
    if (strcmp(operand, "-group") == 0)
    {
        return 1;
    }
    return 0;
}

static struct token *check_op(char *operand, char *value, int flag)
{
    if (strcmp(operand, "-print") == 0)
    {
        return init(NULL, EXPR_PRINT, 1, L);
    }
    if (strcmp(operand, "-name") == 0)
    {
        return init(value, EXPR_NAME, 2, L);
    }
    if (strcmp(operand, "-type") == 0)
    {
        return init(value, EXPR_TYPE, 3, L);
    }
    if (strcmp(operand, "-newer") == 0)
    {
        return init(value, EXPR_NEWER, 4, L);
    }
    if (strcmp(operand, "-perm") == 0)
    {
        return init(value, EXPR_PERM, 4, L);
    }
    if (strcmp(operand, "-user") == 0)
    {
        return init(value, EXPR_USER, 4, L);
    }
    if (strcmp(operand, "-group") == 0)
    {
        return init(value, EXPR_GROUP, 4, L);
    }
    if (strcmp(operand, "(") == 0)
        return init(NULL, LEFT, 4, N);
    if (strcmp(operand, ")") == 0)
        return init(NULL, RIGHT, 4, N);
    if (strcmp(operand, "-o") == 0)
        return init(NULL, OR, 1, L);
    if (strcmp(operand, "-a") == 0)
        return init(NULL, AND, 3, L);
    if (strcmp(operand, "!") == 0)
        return init(NULL, NOT, 3, R);
    else
    {
        if (flag == 0)
        {
            return init(operand, START, 2, L);
        }
    }
    return NULL;
}

static struct token *check_other(char *operand, int flag)
{
    if (strcmp(operand, "-print") == 0)
    {
        return init(NULL, EXPR_PRINT, 1, L);
    }
    if (strcmp(operand, "-d") == 0)
        return init(operand, OPTION, 4, N);
    if (strcmp(operand, "-H") == 0)
        return init(operand, OPTION, 4, N);
    if (strcmp(operand, "-L") == 0)
        return init(operand, OPTION, 1, L);
    if (strcmp(operand, "-P") == 0)
        return init(operand, OPTION, 3, L);
    if (strcmp(operand, "(") == 0)
        return init(NULL, LEFT, 4, N);
    if (strcmp(operand, ")") == 0)
        return init(NULL, RIGHT, 4, N);
    if (strcmp(operand, "-o") == 0)
        return init(NULL, OR, 1, L);
    if (strcmp(operand, "-a") == 0)
        return init(NULL, AND, 3, L);
    if (strcmp(operand, "!") == 0)
        return init(NULL, NOT, 3, R);
    else
    {
        if (flag == 0)
        {
            return init(operand, START, 2, L);
        }
    }
    return NULL;
}

static int handler(struct token ***list, int count_tok, struct token **new,
                   int **no_action)
{
    if (!(*new))
    {
        free_token(*list);
        free(*new);
        return 0;
    }
    if ((*new)->type == EXPR_PRINT)
        *(*no_action) = 1;
    (*list)[count_tok] = (*new);
    return 1;
}

struct token **Tokenise2(int argc, char **argv, int *no_action)
{
    struct token **list = calloc(argc, sizeof(struct token));
    int count_tok = 0;
    int i = 1;
    int startings = 1;
    while (i < argc)
    {
        if (is_operand(argv[i]))
        {
            if (argv[i + 1] != NULL)
            {
                struct token *new = check_op(argv[i], argv[i + 1], 0);
                if (!handler(&list, count_tok, &new, &no_action))
                {
                    free_token(list);
                    return NULL;
                }
                i++;
            }
            else
            {
                free_token(list);
                return NULL;
            }
            startings = 0;
        }
        else if (strcmp(argv[i], "-exec") == 0)
        {
            char *str = calloc(1000, sizeof(char));
            int j = i + 1;
            while (strcmp(argv[j], ";"))
            {
                strcat(str, argv[j++]);
                strcat(str, " ");
            }
            strcat(str, argv[j++]);
            struct token *new = check_op(argv[i], str, 0);
            if (!handler(&list, count_tok, &new, &no_action))
            {
                free_token(list);
                return NULL;
            }
            i = j;
        }
        else
        {
            if ((strcmp(argv[i], "(") != 0) && startings == 1)
            {
                struct token *new = check_other(argv[i], 0);
                if (!handler(&list, count_tok, &new, &no_action))
                {
                    free_token(list);
                    return NULL;
                }
            }
            else
            {
                struct token *new = check_other(argv[i], 1);
                if (!handler(&list, count_tok, &new, &no_action))
                {
                    return NULL;
                }
            }
        }
        count_tok++;
        i++;
    }
    return list;
}

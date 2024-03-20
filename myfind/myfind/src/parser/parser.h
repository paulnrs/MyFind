#ifndef PARSER_H
#define PARSER_H
#include <stddef.h>

enum type
{
    EXPR,
    START,
    EXPR_PRINT,
    EXPR_NAME,
    EXPR_TYPE,
    EXPR_NEWER,
    TYPE,
    OR,
    AND,
    NOT,
    LEFT,
    RIGHT,
    OPTION,
    EXPR_PERM,
    EXPR_USER,
    EXPR_GROUP,
};

enum asso
{
    L,
    R,
    N,
};

enum option
{
    d,
    H,
    l,
    P,
};

struct token
{
    enum type type;
    char *data;
    int prio;
    struct token *left;
    struct token *right;
    enum asso asso;
};

#endif /* ! PARSER_H */

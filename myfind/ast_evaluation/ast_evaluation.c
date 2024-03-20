#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "expression.h"
#include "memory.h"
#include "parser.h"

int eval_expr(struct my_expr *expr)
{
    if (!expr)
    {
        return 0;
    }
    if (expr->type == EXPR_NUMBER)
    {
        return expr->data.value;
    }
    if (expr->type == EXPR_NEGATION)
    {
        return -eval_expr(expr->data.children.left);
    }
    if (expr->type == EXPR_ADDITION)
    {
        return eval_expr(expr->data.children.left)
            + eval_expr(expr->data.children.right);
    }
    if (expr->type == EXPR_SUBTRACTION)
    {
        return eval_expr(expr->data.children.left)
            - eval_expr(expr->data.children.right);
    }
    if (expr->type == EXPR_MULTIPLICATION)
    {
        return eval_expr(expr->data.children.left)
            * eval_expr(expr->data.children.right);
    }
    if (expr->type == EXPR_DIVISION)
    {
        int right = eval_expr(expr->data.children.right);
        if (!right)
        {
            fprintf(stderr, "Division by zero not allowed!\n");
            exit(1);
        }
        return eval_expr(expr->data.children.left)
            / eval_expr(expr->data.children.right);
    }
    return 1;
}

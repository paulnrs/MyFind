#define _POSIX_C_SOURCE 200112L

#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../src/parser/parser.h"
#include "../src/stack/stack.h"

void free_token(struct token **list);
struct stack *shunting(struct token **token, int tok, int prev_operand,
                       int *no_action);
struct token **Tokenise2(int argc, char **argv, int *no_operation);
int simple_ls(char *argv, struct token *token);
int simple_d(char *argv, struct token *token, int option, int start);
int eval_ast(char *path, struct token *token);

static void free_ast(struct token *root)
{
    if (!root)
    {
        return;
    }
    free_ast(root->left);
    free_ast(root->right);
    free(root);
}
/*static void padding ( char ch, int n )
{
  int i;
  for ( i = 0; i < n; i++ )
    putchar ( ch );
}

static void print(struct token *root, int level)
{
    if ( root == NULL ) {
        padding ( '\t', level );
        puts ( "~" );
    }
    else {
    print( root->right, level + 1 );
    padding ( '\t', level );
    switch (root->type)
    {
    case OR:
        printf("OR\n");
        break;
    case AND:
        printf("AND\n");
        break;
    case EXPR_NAME:
        printf("%s\n", root->data);
        break;
    case EXPR_TYPE :
         printf("%s\n", root->data);
        break;
    case EXPR_NEWER :
        printf("%s\n", root->data);
        break;
    case EXPR_PRINT :
        printf("PRINT\n");
        break;
    case EXPR_EXEC :
        printf("%s\n", root->data);
        break;
    default:
        printf ( "%d\n", root->type );
    }
    print( root->left, level + 1 );
    }
}*/

static int is_correct(struct token *root)
{
    if (strcmp(root->data, "f") == 0)
        return 1;
    if (strcmp(root->data, "c") == 0)
        return 1;
    if (strcmp(root->data, "d") == 0)
        return 1;
    if (strcmp(root->data, "l") == 0)
        return 1;
    if (strcmp(root->data, "p") == 0)
        return 1;
    if (strcmp(root->data, "s") == 0)
        return 1;
    else
        return 0;
}

static int is_not(struct token *token)
{
    struct stat h;
    int res2 = lstat(token->data, &h);
    (void)h;
    if (res2 == -1)
    {
        return 0;
    }
    return 1;
}

static int not_user(struct token *token)
{
    struct passwd *pwd = getpwnam(token->data);
    if (!pwd)
    {
        return 0;
    }
    return 1;
}

static int not_group(struct token *token)
{
    struct group *pwd = getgrnam(token->data);
    if (!pwd)
    {
        return 0;
    }
    return 1;
}

static int not_start(struct token *root)
{
    DIR *dir = opendir(root->data);
    if (!dir)
    {
        return 0;
    }
    closedir(dir);
    return 1;
}

/*static int not_operand(struct token *root)
{
    if (!root)
    {
        return 0;
    }
    if (strcmp(root->data, "-name") == 0)
    {
        return 1;
    }
    if (strcmp(root->data, "-type") == 0)
    {
        return 1;
    }
    if (strcmp(root->data, "-newer") == 0)
    {
        return 1;
    }
    if (strcmp(root->data, "-perm") == 0)
    {
        return 1;
    }
    if (strcmp(root->data, "-user") == 0)
    {
        return 1;
    }
    if (strcmp(root->data, "-group") == 0)
    {
        return 1;
    }
    if (root->type == LEFT)
    {
        return 1;
    }
    return 0;
}*/

static int errors(struct token **list, int j)
{
    if (list[j]->type == EXPR_TYPE && !is_correct(list[j]))
    {
        fprintf(stderr, "myfind: invalid type: %s\n", list[j]->data);
        free_token(list);
        return 1;
    }
    if (list[j]->type == EXPR_NEWER && !is_not(list[j]))
    {
        fprintf(stderr, "myfind: invalid file: %s\n", list[j]->data);
        free_token(list);
        return 1;
    }
    if (list[j]->type == EXPR_USER && !not_user(list[j]))
    {
        fprintf(stderr, "myfind: invalid user: %s\n", list[j]->data);
        free_token(list);
        return 1;
    }
    if (list[j]->type == EXPR_GROUP && !not_group(list[j]))
    {
        fprintf(stderr, "myfind: invalid user: %s\n", list[j]->data);
        free_token(list);
        return 1;
    }
    if (list[j]->type == START && !not_start(list[j]))
    {
        fprintf(stderr, "myfind: invalid start: %s\n", list[j]->data);
        free_token(list);
        return 1;
    }
    /*if ((list[j]->type == AND || list[j]->type == OR) &&
    !not_operand(list[j]))
    {
        fprintf(stderr, "myfind: invalid AND: %s\n", list[j]->data);
        free_token(list);
        return 1;
    }*/
    return 0;
}

static int good_option(int k, struct token **list)
{
    int option = 0;
    for (int i = 0; i < k; i++)
    {
        if (strcmp(list[i]->data, "-d") == 0)
        {
            option = option | 1;
        }
        if (strcmp(list[i]->data, "-l") == 0)
        {
            option = option & 1;
            option = option | 2;
        }
        if (strcmp(list[i]->data, "-H") == 0)
        {
            option = option & 1;
            option = option | 4;
        }
        if (strcmp(list[i]->data, "-P") == 0)
        {
            option = option & 1;
            option = option | 8;
        }
    }
    return option;
}
static void free_left(struct token **list)
{
    for (int i = 0; list[i]; i++)
    {
        if (list[i]->type == OPTION || list[i]->type == LEFT
            || list[i]->type == RIGHT || list[i]->type == START)
        {
            free(list[i]);
        }
    }
}

int main(int argc, char **argv)
{
    int no_action = 0;
    struct token **list = Tokenise2(argc, argv, &no_action);
    if (!list)
    {
        fprintf(stderr, "myfind: invalid number of args.\n");
        return 1;
    }
    else
    {
        int i = 0;
        int k = 0;
        for (int j = 0; list[j]; j++)
        {
            if (errors(list, j))
                return 1;
            if (list[j]->type == START)
                i++;
            if (list[j]->type == OPTION)
                k++;
        }
        struct stack *new = shunting(list, i + k, 1, &no_action);
        struct token *peek = stack_peek(new);
        // print(peek, i);
        int option = good_option(k, list);
        if (i == 0)
        {
            simple_d(".", peek, option, 1);
        }
        else
        {
            for (int j = k; j < k + i; j++)
            {
                simple_d(list[j]->data, peek, option, 1);
                // free(list[j]);
            }
        }
        new = stack_pop(new);
        free_left(list);
        free(new);
        free(list);
        free_ast(peek);
    }
    return 0;
}

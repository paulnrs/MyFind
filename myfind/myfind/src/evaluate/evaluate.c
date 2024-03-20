#define _POSIX_C_SOURCE 200112L

#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fnmatch.h>
#include <grp.h>
#include <libgen.h>
#include <pwd.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../parser/parser.h"
#include "../stack/stack.h"

typedef int (*const operation)(char *, char *, struct token *root);

static int simple_print(char *path, char *name, struct token *root)
{
    (void)name;
    (void)root;
    printf("%s\n", path);
    return 1;
}

static int name(char *path, char *name, struct token *root)
{
    (void)name;
    char *base = basename(path);
    return (fnmatch(root->data, base, 0) == 0);
}

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

static int is_type(struct token *root, struct stat *s)
{
    if (strcmp(root->data, "f") == 0 && S_ISREG(s->st_mode))
        return 1;
    if (strcmp(root->data, "b") == 0 && S_ISBLK(s->st_mode))
        return 1;
    if (strcmp(root->data, "c") == 0 && S_ISCHR(s->st_mode))
        return 1;
    if (strcmp(root->data, "d") == 0 && S_ISDIR(s->st_mode))
        return 1;
    if (strcmp(root->data, "l") == 0 && S_ISLNK(s->st_mode))
        return 1;
    if (strcmp(root->data, "p") == 0 && S_ISFIFO(s->st_mode))
        return 1;
    if (strcmp(root->data, "s") == 0 && S_ISSOCK(s->st_mode))
        return 1;
    else if (!is_correct(root))
        return 2;
    return 0;
}

static int type(char *path, char *name, struct token *root)
{
    struct stat s;
    int res = lstat(path, &s);
    if (res == -1)
    {
        fprintf(stderr, "myfind : oui\n");
        return 2;
    }
    (void)name;
    (void)root;
    int result = is_type(root, &s);
    if (result == 2)
    {
        fprintf(stderr, "myfind : Invalid non\n");
        return 2;
    }
    else
    {
        return result;
    }
}

static int newer(char *path, char *name, struct token *root)
{
    struct stat s;
    (void)name;
    int res = lstat(path, &s);
    (void)res;
    struct stat h;
    int res2 = lstat(root->data, &h);
    (void)res2;
    return s.st_mtime > h.st_mtime;
}

static int perm(char *path, char *name, struct token *root)
{
    struct stat s;
    (void)name;
    int res = lstat(path, &s);
    (void)res;
    if (root->data[0] == '/')
    {
        unsigned long data = strtoul(root->data + 1, NULL, 8);
        return ((511 & s.st_mode) & data) > 0;
    }
    if (root->data[0] == '-')
    {
        unsigned long data = strtoul(root->data + 1, NULL, 8);
        return ((511 & s.st_mode) & data) == data;
    }
    else
    {
        unsigned long data = strtoul(root->data, NULL, 8);
        return (511 & s.st_mode) == data;
    }
}

static int user(char *path, char *name, struct token *root)
{
    (void)root;
    struct stat s;
    int res = lstat(path, &s);
    (void)res;
    (void)name;
    struct passwd *pwd = getpwuid(s.st_uid);
    return (strcmp(pwd->pw_name, root->data) == 0);
}

static int group(char *path, char *name, struct token *root)
{
    struct stat s;
    int res = lstat(path, &s);
    (void)res;
    (void)name;
    struct group *pwd = getgrgid(s.st_gid);
    return (strcmp(pwd->gr_name, root->data) == 0);
}

static const operation operations[] = {
    [2] = simple_print, [3] = name,  [4] = type,   [5] = newer,
    [13] = perm,        [14] = user, [15] = group,
};

int eval_ast(char *path, char *name, struct token *root)
{
    if (!root)
    {
        return 1;
    }
    if (root->type == OR)
    {
        return eval_ast(path, name, root->left)
            || eval_ast(path, name, root->right);
    }
    else if (root->type == AND)
    {
        return eval_ast(path, name, root->left)
            && eval_ast(path, name, root->right);
    }
    else if (root->type == NOT)
    {
        return !eval_ast(path, name, root->right);
    }
    else
    {
        return operations[root->type](path, name, root);
    }
}

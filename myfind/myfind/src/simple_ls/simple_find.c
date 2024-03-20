#define _POSIX_C_SOURCE 200112L

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../parser/parser.h"
#include "../stack/stack.h"

#define DT_DIR 4

int eval_ast(char *path, char *name, struct token *token);

static void handle_path(char *path, char *argv, struct dirent *r)
{
    strcpy(path, argv);
    int last = strlen(path) - 1;
    if (path[last] != '/')
        strcat(path, "/");
    strcat(path, r->d_name);
}

int simple_d(char *argv, struct token *token, int option, int start)
{
    DIR *dir = opendir(argv);
    if (!dir)
    {
        fprintf(stderr, "simple_ls: %s, no such file or directory\n", argv);
        return 1;
    }
    struct dirent *r = readdir(dir);
    if (!(option & 1))
    {
        eval_ast(argv, argv, token);
    }
    for (; r; r = readdir(dir))
    {
        struct stat s;
        int res = lstat(argv, &s);
        (void)res;
        if (r->d_type == DT_DIR || ((option & 2) && S_ISLNK(s.st_mode))
            || ((option & 3) && S_ISLNK(s.st_mode) && start == 1))
        {
            if (!strcmp(r->d_name, ".") || !strcmp(r->d_name, ".."))
            {
                continue;
            }
            char path[500];
            handle_path(path, argv, r);
            if (simple_d(path, token, option, 0))
                continue;
        }
        else
        {
            char path[500];
            handle_path(path, argv, r);
            eval_ast(path, r->d_name, token);
        }
    }
    if (option & 1)
        eval_ast(argv, argv, token);
    closedir(dir);
    return 0;
}

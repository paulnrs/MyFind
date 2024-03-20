#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>

int simple_ls(char *argv)
{
    DIR *dir = opendir(argv);
    if (!dir)
    {
        fprintf(stderr, "simple_ls: %s, no such file or directory", argv);
        return 1;
    }
    struct dirent *r = readdir(dir);
    while (r)
    {
        if (r->d_name[0] != '.')
        {
            printf("%s\n", r->d_name);
        }
        r = readdir(dir);
    }
    closedir(dir);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        if (simple_ls("."))
        {
            return 1;
        }
    }
    else
    {
        if (simple_ls(argv[1]))
        {
            return 1;
        }
    }
    return 0;
}

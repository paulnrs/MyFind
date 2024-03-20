#include <err.h>
#include <stdio.h>
#include <sys/stat.h>

int simple_stat(char *argv)
{
    struct stat s;
    int res = stat(argv, &s);
    if (res == -1)
    {
        return 1;
    }
    printf("st_dev=%ld\n", s.st_dev);
    printf("st_ino=%ld\n", s.st_ino);
    printf("st_mode=0%o\n", s.st_mode);
    printf("st_nlink=%ld\n", s.st_nlink);
    printf("st_uid=%d\n", s.st_uid);
    printf("st_gid=%d\n", s.st_gid);
    printf("st_rdev=%ld\n", s.st_rdev);
    printf("st_size=%ld\n", s.st_size);
    printf("st_atime=%ld\n", s.st_atime);
    printf("st_mtime=%ld\n", s.st_mtime);
    printf("st_ctime=%ld\n", s.st_ctime);
    printf("st_blksize=%ld\n", s.st_blksize);
    printf("st_blocks=%ld\n", s.st_blocks);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc == 0)
    {
        return 1;
    }
    for (int i = 1; i < argc; i++)
    {
        if (simple_stat(argv[i]))
        {
            return 1;
        }
    }
    return 0;
}

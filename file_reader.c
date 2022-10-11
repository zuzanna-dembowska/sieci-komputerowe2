#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX 512

int main(int argc, char *argv[])
{
    char buf[MAX];
    int source_fd;
    int read_bytes;

    if (argc < 2)
    {
        fprintf(stderr, "Too few parameters. Use:\n");
        fprintf(stderr, "%s <source file>\n", argv[0]);
        exit(1);
    }

    source_fd = open(argv[1], O_RDONLY);
    if (source_fd == -1)
    {
        perror("Source file open error");
        exit(1);
    }

    while ((read_bytes = read(source_fd, buf, MAX)) > 0)
    {
        printf("%s\n", buf);
    }

    if (read_bytes == -1)
    {
        perror("Source file read error");
        exit(1);
    }

    if (close(source_fd) == -1)
    {
        perror("File close error");
        exit(1);
    }

    exit(0);
}
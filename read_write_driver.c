#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main()
{
    int fd;
    char write_buffer[] = "hello!";
    char read_buffer[1024];
    fd = open("/workspaces/Character-Device-Driver/driver.ko", O_RDWR);

    if(fd < 0)
    {
        printf("can't open the driver...\n");
        return 0;
    }

    if (write(fd, write_buffer, strlen(write_buffer) + 1) < 0) 
    {
        printf("failed to write to driver\n");
        return 0;
    }

    if (read(fd, read_buffer, 1024) < 0) {
        printf("failed to read from driver\n");
        return 0;
    }

    write(fd, write_buffer, strlen(write_buffer) + 1);
    read(fd, read_buffer, 1024);
    close(fd);

    printf("%s \n", read_buffer);
}


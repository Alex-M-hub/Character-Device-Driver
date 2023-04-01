#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main()
{
    int fd; //Declares an integer variable fd as an identifier for a file descriptor.
    char write_buffer[] = "hello!"; //initializes a character array with the value "hello!"
    char read_buffer[1024]; //initializes a character array, with a size of 1024 character.
    fd = open("/workspaces/Character-Device-Driver/driver", O_RDWR);
    //open a kernel module file located at "/workspaces/Character-Device-Driver/driver.ko" with read and write access, and sets the returned file descriptor to the fd variable.
    if(fd < 0) //Checks if the fd is less than 0
    {
        printf("can't open the driver...\n"); //prints an error message if it failed.
        return 0;
    }

    if (write(fd, write_buffer, strlen(write_buffer) + 1) < 0) //If the write operation fails
    {
        printf("failed to write to driver\n"); //prints an error message.
        return 0;
    }

    if (read(fd, read_buffer, 1024) < 0) ////If the read operation fails
    {
        printf("failed to read from driver\n"); //prints an error message.
        return 0;
    }

    write(fd, write_buffer, strlen(write_buffer) + 1); //Writes the contents of the write_buffer to the driver
    read(fd, read_buffer, 1024); //Reads the data from the driver
    close(fd); //Closes the file descriptor

    printf("%s \n", read_buffer); //prints the content of the read_buffer.
}
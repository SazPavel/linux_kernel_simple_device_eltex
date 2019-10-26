#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "simple_driver.h"

int main(int argc, char **argv)
{
    int command, number;
    int fd = open("/dev/simple_device", O_RDWR);
    if(fd < 0)
    {
         perror("open file");
         exit(-1);
    }
    if(argc < 2)
    {
        printf("./main command(1 - write, 2 - read) write number\n");
        exit(-1);
    }
    sscanf(argv[1], "%d", &command);

    switch(command)
    {
        case 1:
            if(argc < 3)
                number = 3;
            else
                sscanf(argv[2], "%d", &number);
            if(ioctl(fd, SIMPLE_DRIVER_WRITE, &number) < 0)
            {
                perror("ioctl write");
                exit(-1);
            }
            break;
        case 2:
            if(ioctl(fd, SIMPLE_DRIVER_READ, &number) < 0)
            {
                perror("ioctl read");
                exit(-1);
            }
            printf("simple_num_for_ioctl: %d\n", number);
            break;
    }
    exit(0);
}

#include <stdio.h>  // Allowsus to get input/output, like printf, scanf, fopen
#include <stdlib.h> // Allows us to use malloc, atoi, strtol, exit (For dynamic memory)
#include <string.h> // strlen, strcmp, strcpy

#define BUFFER_SIZE 1024

int main()
{
    FILE *fptr = fopen("/proc/meminfo", "r"); // we open the file in proc to get memory info from the kernal
    if (fptr == NULL)
    {
        fprintf(stderr, "Error, File /proc/meminfo could not be opened");
    }

    char buff[BUFFER_SIZE];

    while (fgets(buff, BUFFER_SIZE, fptr))
    {
        if (strncmp("MemTotal", buff, 8) == 0)
        {
            printf("%s", buff);
        }
        else if (strncmp("MemFree", buff, 7) == 0)
        {
            printf("%s", buff);
        }
        else if (strncmp("MemAvailable", buff, 12) == 0)
        {
            printf("%s", buff);
        }
        else
            printf("Misc Information\n");
    }

    fclose(fptr);
    return 0;
}
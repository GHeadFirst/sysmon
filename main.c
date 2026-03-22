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
        return -1;
    }

    char buff[BUFFER_SIZE];

    unsigned long total_memory, free_memory, available_memory;
    char temp[100];

    while (fgets(buff, BUFFER_SIZE, fptr))
    {
        if (strncmp("MemTotal", buff, 8) == 0)
        {
            sscanf(buff, "%s %lu", temp, &total_memory);
        }
        else if (strncmp("MemFree", buff, 7) == 0)
        {
            sscanf(buff, "%s %lu", temp, &free_memory);
        }
        else if (strncmp("MemAvailable", buff, 12) == 0)
        {
            sscanf(buff, "%s %lu", temp, &available_memory);
        }
        else
        {
            // printf("Misc Information\n");
        }
    }

    total_memory = total_memory / 1024;

    free_memory = free_memory / 1024;

    available_memory = available_memory / 1024;

    printf("=== Memory Info ===\n");

    printf("%-20s %10lu MB\n", "Total Memory:", total_memory);

    printf("%-20s %10lu MB\n", "Free Memory:", free_memory);

    printf("%-20s %10lu MB\n", "Available Memory:", available_memory);

    // printf("Temp variable %s\n", temp);

    total_memory = total_memory / 1024;

    // printf("Test output, my memory is: %luGB\n", total_memory);

    fclose(fptr);

    fptr = fopen("/proc/cpuinfo", "r");
    if (!fptr)
    {
        fprintf(stderr, "Error opening file /proc/cpuinfo");
        return -1;
    }

    char cpu_model_name[256];
    int core_count;

    while (fgets(buff, BUFFER_SIZE, fptr))
    {
        if (strncmp("model name", buff, 10) == 0)
        {
            char *starting_point = strchr(buff, ':'); // basically here it searches inside my buffer until it finds where ":" starts and returns a pointer to that position ->model name : blah blah blah, it returns the position of ":" in that line/buffer
            if (starting_point)                       // this is supposed to be the pointer at where : is located in my buffer
            {
                starting_point++;
                strncpy(cpu_model_name, starting_point, 256);
            }
        }
        else if (strncmp("cpu cores", buff, 9) == 0)
        {
            char *starting_point = strchr(buff, ':');
            if (starting_point)
            {
                starting_point++;
                while (*starting_point == ' ')
                {
                    starting_point++;
                }

                sscanf(starting_point, "%d", &core_count);
                break;
            }
        }
        else
        {
        }
    }

    printf("=== CPU Info ===\n");
    printf("CPU Model Name:%-15s", cpu_model_name);

    printf("CPU Core Count:%-15d\n", core_count);
    return 0;
}
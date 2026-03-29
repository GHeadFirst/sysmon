#include <stdio.h>  // Allowsus to get input/output, like printf, scanf, fopen
#include <stdlib.h> // Allows us to use malloc, atoi, strtol, exit (For dynamic memory)
#include <string.h> // strlen, strcmp, strcpy

#define BUFFER_SIZE 1024

struct Interface;

int read_memory_info(unsigned long *total_memory, unsigned long *free_memory, unsigned long *available_memory);
int read_cpu_info(char *model_name, int *core_count);
int read_uptime(double *uptime, double *idle_time);
int read_net_dev();

struct Interface create_interface(char interface_name[64], unsigned long received_bytes, unsigned long received_packets, unsigned long received_errors, unsigned long received_drops, unsigned long received_fifo, unsigned long received_frame, unsigned long received_compressed, unsigned long received_multicast, unsigned long transmitted_bytes, unsigned long transmitted_packets, unsigned long transmitted_errors, unsigned long transmitted_drops, unsigned long transmitted_fifo, unsigned long transmitted_colls, unsigned long transmitted_carrier, unsigned long transmitted_compressed);

int main()
{
  unsigned long total_memory, free_memory, available_memory;
  int read_memory = read_memory_info(&total_memory, &free_memory, &available_memory);
  if (read_memory >= 0)
  {

    printf("=== Memory Info ===\n");

    printf("%-20s %10lu MB\n", "Total Memory:", total_memory);

    printf("%-20s %10lu MB\n", "Free Memory:", free_memory);

    printf("%-20s %10lu MB\n", "Available Memory:", available_memory);
  }
  // printf("Temp variable %s\n", temp);

  // printf("Test output, my memory is: %luGB\n", total_memory);

  char cpu_model_name[256];
  int core_count;
  read_cpu_info(cpu_model_name, &core_count);

  printf("=== CPU Info ===\n");

  printf("CPU Model Name:%5s", cpu_model_name);

  printf("CPU Core Count:%5d\n", core_count);

  /*   printf("=== CPU Info ===\n");

    printf("CPU Model Name:%5s", cpu_model_name);

    printf("CPU Core Count:%5d\n", core_count); */

  printf("=== Runtime Info ===\n");

  double system_uptime, idle_time;
  read_uptime(&system_uptime, &idle_time);

  int system_uptime_hour = system_uptime / 3600;

  int system_uptime_minutes = ((int)system_uptime % 3600) / 60;

  int system_idle_time_hour = idle_time / 3600;

  int system_idle_time_minutes = ((int)idle_time % 3600) / 60;

  printf("System Uptime: %dH : %dM\n", system_uptime_hour,
         system_uptime_minutes);

  printf("System Idle Time: %dH : %dM\n", system_idle_time_hour,
         system_idle_time_minutes);

  read_net_dev();
  return 0;
}

int read_memory_info(unsigned long *total_memory, unsigned long *free_memory, unsigned long *available_memory)
{
  FILE *fptr =
      fopen("/proc/meminfo",
            "r"); // we open the file in proc to get memory info from the kernal
  if (fptr == NULL)
  {
    fprintf(stderr, "Error, File /proc/meminfo could not be opened");
    return -1;
  }

  char buff[BUFFER_SIZE];

  while (fgets(buff, BUFFER_SIZE, fptr))
  {
    char *starting_point = strchr(buff, ':');
    if (strncmp("MemTotal", buff, 8) == 0)
    {
      if (starting_point)
      {
        starting_point++;
        sscanf(starting_point, "%lu", total_memory);
      }
      // sscanf(starting_point, "%lu", total_memory);
    }
    else if (strncmp("MemFree", buff, 7) == 0)
    {
      if (starting_point)
      {
        starting_point++;
        sscanf(starting_point, "%lu", free_memory);
      }
      // sscanf(starting_point, "%lu", free_memory);
    }
    else if (strncmp("MemAvailable", buff, 12) == 0)
    {
      if (starting_point)
      {
        starting_point++;
        sscanf(starting_point, "%lu", available_memory);
      }
      // sscanf(starting_point, "%lu", available_memory);
    }
  }
  fclose(fptr);
  // to convert to MB instead of KB
  *total_memory = *total_memory / 1024;
  *free_memory = *free_memory / 1024;
  *available_memory = *available_memory / 1024;

  return 0;
}

int read_cpu_info(char *model_name, int *core_count)
{
  char buff[BUFFER_SIZE];

  FILE *fptr = fopen("/proc/cpuinfo", "r");

  if (!fptr)
  {

    fprintf(stderr, "Error opening file /proc/cpuinfo");

    return -1;
  }

  while (fgets(buff, BUFFER_SIZE, fptr))

  {

    if (strncmp("model name", buff, 10) == 0)
    {

      char *starting_point = strchr(

          buff, ':'); // basically here it searches inside my buffer until it

      // finds where ":" starts and returns a pointer to that

      // position ->model name : blah blah blah, it returns the

      // position of ":" in that line/buffer

      if (starting_point) // this is supposed to be the pointer at where : is

      // located in my buffer

      {

        starting_point++;

        strncpy(model_name, starting_point, 256);
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

        sscanf(starting_point, "%d", core_count);

        break;
      }
    }
  }

  fclose(fptr);
}

int read_uptime(double *uptime, double *idle_time)
{
  char buff[BUFFER_SIZE];

  FILE *fptr = fopen("/proc/uptime", "r");
  if (!fptr)
  {
    fprintf(stderr, "Error Opening File /proc/uptime");
    return -1;
  }
  while (fgets(buff, BUFFER_SIZE, fptr))
  {
    sscanf(buff, "%lf %lf", uptime, idle_time);
  }
  fclose(fptr);
}

int read_net_dev()
{

  char buff[BUFFER_SIZE];
  FILE *fptr = fopen("/proc/net/dev", "r");

  if (!fptr)
  {
    fprintf(stderr, "Error Opening File /proc/net/dev");
    return -1;
  }

  char interface_name[64];
  unsigned long received_bytes, received_packets, received_errors, received_drops, received_fifo, received_frame, received_compressed, received_multicast;
  unsigned long transmitted_bytes, transmitted_packets, transmitted_errors, transmitted_drops, transmitted_fifo, transmitted_colls, transmitted_carrier, transmitted_compressed;
  while (fgets(buff, BUFFER_SIZE, fptr))
  {
    char *target = strchr(buff, ':');
    if (!target)
    {
      continue;
    }

    strncpy(interface_name, buff, target - buff);
    interface_name[target - buff] = '\0';
    target++;
    sscanf(target, "%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu", &received_bytes, &received_packets, &received_errors, &received_drops, &received_fifo, &received_frame, &received_compressed, &received_multicast, &transmitted_bytes, &transmitted_packets, &transmitted_errors, &transmitted_drops, &transmitted_fifo, &transmitted_colls, &transmitted_carrier, &transmitted_compressed);
    printf("My Received Bytes are: %lu\n My Received Packets are: %lu \n My received Errors: %lu \n Received Drops: %lu \n Received FIFO: %lu \n Received frame : %lu \n Received Compressed: %lu \n Received multicast: %lu \n", received_bytes, received_packets, received_errors, received_drops, received_fifo, received_frame, received_compressed, received_multicast);
    printf("My Transmitted Bytes are: %lu\n My Transmitted Packets are: %lu \n My Transmitted Errors: %lu \n Transmitted Drops: %lu \n Transmitted FIFO: %lu \n Transmitted colls : %lu \n Transmitted carrier: %lu \n Transmitted compressed: %lu \n", transmitted_bytes, transmitted_packets, transmitted_errors, transmitted_drops, transmitted_fifo, transmitted_colls, transmitted_carrier, transmitted_compressed);
  }

  fclose(fptr);
}

struct Interface
{
  char interface_name[64];
  unsigned long received_bytes, received_packets, received_errors, received_drops, received_fifo, received_frame, received_compressed, received_multicast;
  unsigned long transmitted_bytes, transmitted_packets, transmitted_errors, transmitted_drops, transmitted_fifo, transmitted_colls, transmitted_carrier, transmitted_compressed;
};

struct Interface create_interface(char interface_name[64], unsigned long received_bytes, unsigned long received_packets, unsigned long received_errors, unsigned long received_drops, unsigned long received_fifo, unsigned long received_frame, unsigned long received_compressed, unsigned long received_multicast, unsigned long transmitted_bytes, unsigned long transmitted_packets, unsigned long transmitted_errors, unsigned long transmitted_drops, unsigned long transmitted_fifo, unsigned long transmitted_colls, unsigned long transmitted_carrier, unsigned long transmitted_compressed)
{
  struct Interface inter;
  strcpy(inter.interface_name, interface_name);
  inter.received_bytes = received_bytes;
  inter.received_packets = received_packets;
  inter.received_errors = received_errors;
  inter.received_drops = received_drops;
  inter.received_fifo = received_fifo;
  inter.received_frame = received_frame;
  inter.received_compressed = received_compressed;
  inter.received_multicast = received_multicast;

  inter.transmitted_bytes = transmitted_bytes;
  inter.transmitted_packets = transmitted_packets;
  inter.transmitted_errors = transmitted_errors;
  inter.transmitted_drops = transmitted_drops;
  inter.transmitted_fifo = transmitted_fifo;
  inter.transmitted_colls = transmitted_colls;
  inter.transmitted_carrier = transmitted_carrier;
  inter.transmitted_compressed = transmitted_compressed;
  return inter;
}

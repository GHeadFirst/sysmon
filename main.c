#include <stdio.h>  // Allowsus to get input/output, like printf, scanf, fopen
#include <stdlib.h> // Allows us to use malloc, atoi, strtol, exit (For dynamic memory)
#include <string.h> // strlen, strcmp, strcpy
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

#define BUFFER_SIZE 1024
#define MAX_INTERFACES 16

struct Interface
{
  char name[64];
  unsigned long received_bytes;
  unsigned long received_packets;
  unsigned long received_errors;
  unsigned long received_drops;
  unsigned long received_fifo;
  unsigned long received_frame;
  unsigned long received_compressed;
  unsigned long received_multicast;
  unsigned long transmitted_bytes;
  unsigned long transmitted_packets;
  unsigned long transmitted_errors;
  unsigned long transmitted_drops;
  unsigned long transmitted_fifo;
  unsigned long transmitted_colls;
  unsigned long transmitted_carrier;
  unsigned long transmitted_compressed;
};

int read_memory_info(unsigned long *total_memory, unsigned long *free_memory, unsigned long *available_memory);
int read_cpu_info(char *model_name, int *core_count);

int read_uptime(double *uptime, double *idle_time);

int read_net_dev(struct Interface *interface_array, int *interface_count);

struct Interface create_interface(char *name, unsigned long received_bytes, unsigned long received_packets, unsigned long received_errors, unsigned long received_drops, unsigned long received_fifo, unsigned long received_frame, unsigned long received_compressed, unsigned long received_multicast, unsigned long transmitted_bytes, unsigned long transmitted_packets, unsigned long transmitted_errors, unsigned long transmitted_drops, unsigned long transmitted_fifo, unsigned long transmitted_colls, unsigned long transmitted_carrier, unsigned long transmitted_compressed);

int main()
{
  int program_errors = 0;
  unsigned long total_memory, free_memory, available_memory;
  int read_memory = read_memory_info(&total_memory, &free_memory, &available_memory);
  if (read_memory >= 0)
  {

    printf("=== Memory Info ===\n");

    printf("%-20s %10lu MB\n", "Total Memory:", total_memory);

    printf("%-20s %10lu MB\n", "Free Memory:", free_memory);

    printf("%-20s %10lu MB\n", "Available Memory:", available_memory);
  }
  else
  {
    fprintf(stderr, "Error opening file /proc/meminfo");
  }
  // printf("Temp variable %s\n", temp);

  // printf("Test output, memory is: %luGB\n", total_memory);

  char cpu_model_name[256];
  int core_count;
  int read_cpu = read_cpu_info(cpu_model_name, &core_count);

  if (read_cpu >= 0)
  {

    printf("=== CPU Info ===\n");

    printf("CPU Model Name:%5s", cpu_model_name);

    printf("CPU Core Count:%5d\n", core_count);
  }
  else
  {
    fprintf(stderr, "Error opening file /proc/cpuinfo");
    program_errors++;
  }

  double system_uptime, idle_time;
  int uptime_code = read_uptime(&system_uptime, &idle_time);
  if (uptime_code >= 0)
  {
    printf("=== Runtime Info ===\n");

    int system_uptime_hour = system_uptime / 3600;

    int system_uptime_minutes = ((int)system_uptime % 3600) / 60;

    int system_idle_time_hour = idle_time / 3600;

    int system_idle_time_minutes = ((int)idle_time % 3600) / 60;
    printf("System Uptime: %dH : %dM\n", system_uptime_hour,
           system_uptime_minutes);

    printf("System Idle Time: %dH : %dM\n", system_idle_time_hour,
           system_idle_time_minutes);
  }
  else
  {
    fprintf(stderr, "Error Opening File /proc/uptime");
    program_errors++;
  }

  struct Interface interface_array[16];

  int interface_count = 0;

  int net_dev_code = read_net_dev(interface_array, &interface_count);

  if (net_dev_code >= 0)
  {
    for (size_t i = 0; i < interface_count && i < MAX_INTERFACES; i++)
    {
      struct Interface inter = interface_array[i];

      printf("====== Interface Name: %s ======\n", inter.name);
      printf("Received Bytes: %lu\t Received Packets: %lu \t Received Errors: %lu \t Received Drops: %lu \t Received FIFO: %lu \t Received frame : %lu \t Received Compressed: %lu \t Received multicast: %lu \t", inter.received_bytes, inter.received_packets, inter.received_errors, inter.received_drops, inter.received_fifo, inter.received_frame, inter.received_compressed, inter.received_multicast);
      printf("Transmitted Bytes: %lu\t Transmitted Packets: %lu \t Transmitted Errors: %lu \t Transmitted Drops: %lu \t Transmitted FIFO: %lu \t Transmitted colls : %lu \t Transmitted carrier: %lu \t Transmitted compressed: %lu \n", inter.transmitted_bytes, inter.transmitted_packets, inter.transmitted_errors, inter.transmitted_drops, inter.transmitted_fifo, inter.transmitted_colls, inter.transmitted_carrier, inter.transmitted_compressed);
    }
  }
  else
  {
    fprintf(stderr, "Error Opening File /proc/net/dev");
    program_errors++;
  }
  return program_errors;

  int socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8080);

  bind(server_fd, (struct sockaddr *)&address, sizeof(address));
}

int read_memory_info(unsigned long *total_memory, unsigned long *free_memory, unsigned long *available_memory)
{
  FILE *fptr =
      fopen("/proc/meminfo",
            "r"); // we open the file in proc to get memory info from the kernal
  if (fptr == NULL)
  {
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
    return -1;
  }
  while (fgets(buff, BUFFER_SIZE, fptr))
  {
    sscanf(buff, "%lf %lf", uptime, idle_time);
  }
  fclose(fptr);
}

int read_net_dev(struct Interface *interface_array, int *interface_count)
{

  char buff[BUFFER_SIZE];
  FILE *fptr = fopen("/proc/net/dev", "r");

  if (!fptr)
  {
    return -1;
  }

  while (fgets(buff, BUFFER_SIZE, fptr))
  {
    char interface_name[64];

    unsigned long received_bytes, received_packets, received_errors, received_drops, received_fifo, received_frame, received_compressed, received_multicast;

    unsigned long transmitted_bytes, transmitted_packets, transmitted_errors, transmitted_drops, transmitted_fifo, transmitted_colls, transmitted_carrier, transmitted_compressed;

    char *target = strchr(buff, ':');

    if (!target)
    {
      continue;
    }

    strncpy(interface_name, buff, target - buff);
    interface_name[target - buff] = '\0';
    target++;
    sscanf(target, "%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu", &received_bytes, &received_packets, &received_errors, &received_drops, &received_fifo, &received_frame, &received_compressed, &received_multicast, &transmitted_bytes, &transmitted_packets, &transmitted_errors, &transmitted_drops, &transmitted_fifo, &transmitted_colls, &transmitted_carrier, &transmitted_compressed);

    struct Interface inter = create_interface(interface_name, received_bytes, received_packets, received_errors, received_drops, received_fifo, received_frame, received_compressed, received_multicast, transmitted_bytes, transmitted_packets, transmitted_errors, transmitted_drops, transmitted_fifo, transmitted_colls, transmitted_carrier, transmitted_compressed);
    interface_array[*interface_count] = inter;
    *interface_count = *interface_count + 1;

    /*     printf("====== Interface Name: %s ======\n", interface_name);
        printf("Received Bytes: %lu\n Received Packets: %lu \n Received Errors: %lu \n Received Drops: %lu \n Received FIFO: %lu \n Received frame : %lu \n Received Compressed: %lu \n Received multicast: %lu \n", received_bytes, received_packets, received_errors, received_drops, received_fifo, received_frame, received_compressed, received_multicast);
        printf("Transmitted Bytes: %lu\n Transmitted Packets: %lu \n Transmitted Errors: %lu \n Transmitted Drops: %lu \n Transmitted FIFO: %lu \n Transmitted colls : %lu \n Transmitted carrier: %lu \n Transmitted compressed: %lu \n", transmitted_bytes, transmitted_packets, transmitted_errors, transmitted_drops, transmitted_fifo, transmitted_colls, transmitted_carrier, transmitted_compressed); */
  }

  fclose(fptr);
  return 0;
}

struct Interface create_interface(char *name, unsigned long received_bytes, unsigned long received_packets, unsigned long received_errors, unsigned long received_drops, unsigned long received_fifo, unsigned long received_frame, unsigned long received_compressed, unsigned long received_multicast, unsigned long transmitted_bytes, unsigned long transmitted_packets, unsigned long transmitted_errors, unsigned long transmitted_drops, unsigned long transmitted_fifo, unsigned long transmitted_colls, unsigned long transmitted_carrier, unsigned long transmitted_compressed)
{
  struct Interface inter;

  strncpy(inter.name, name, 64);
  inter.name[63] = '\0';
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

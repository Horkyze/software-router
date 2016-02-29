#define LOG_FILE "log.txt"
#define FILE_LENGTH 0x100
#define SHARED_FILE "/tmp/3948hf"

#include <pcap.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdarg.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "functions.h"




int main (int argc, char* const argv[])
{
  int fd;
  void * file_memory;
  char * string = malloc(50);

  /* Open the file. */
  fd = open (SHARED_FILE, O_RDWR, S_IRUSR | S_IWUSR);

  /* Create the memory mapping. */
  file_memory = mmap (0, FILE_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close (fd);
  printf("file_memory: %p\n",file_memory );

  /* Read the integer, print it out, and double it. */
  //fscanf (file_memory, "%s", string);
  while(1) printf ("value: %s\n", file_memory);
  //sprintf ((char*) file_memory, "%d\n", 2 * integer);

  /* Release the memory (unnecessary because the program exits). */
  munmap (file_memory, FILE_LENGTH);
  return 0;
}

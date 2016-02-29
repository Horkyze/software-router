#ifndef MAPPED_MEM_H
#define MAPPED_MEM_H

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define MEM_IN 1
#define MEM_OUT 2
#define MEM_IN_FILE  "/tmp/mapped_in"
#define MEM_OUT_FILE "/tmp/mapped_out"
#define FILE_LENGTH 0x1000

typedef enum exec_status {
    NO, YES, ERROR
}exec_status;

typedef struct input_cmd_s{
    int executed; //YES, NO, ERROR
    char input_cmd[100];
    char args[100];
}input_cmd_s;


input_cmd_s * input_mem = 0;


void read_mapped_mem(char * mem_file){
    int fd;
    void * file_memory;
    char * string = malloc(50);
    char file_data[FILE_LENGTH];

    /* Open the file. */
    fd = open (mem_file, O_RDWR, S_IRUSR | S_IWUSR);

    /* Create the memory mapping. */
    file_memory = mmap (0, FILE_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close (fd);

    input_mem = (input_cmd_s *) file_memory;

    // sprintf(log_b, "file_memory:  '%s'\n",file_memory );
    // my_log(log_b);
    strcpy(file_data, file_memory);
    sscanf (file_data,      "%d \n",&input_mem->executed);
    sscanf (file_data + 2,  " %[^\n]", input_mem->input_cmd);
    sscanf (strchr(file_data + 2, '\n'), " %[^\n]", input_mem->args);


    sprintf(log_b, "file_data:  '%s'\n",file_data );
    my_log(log_b);

    sprintf(log_b, "executed:  %i\n", input_mem->executed );
    my_log(log_b);
    sprintf(log_b, "input:    '%s'\n", input_mem->input_cmd );
    my_log(log_b);
    sprintf(log_b, "args:     '%s'\n", input_mem->args );
    my_log(log_b);


    /* Release the memory (unnecessary because the program exits). */
    //munmap (file_memory, FILE_LENGTH);

}

#endif

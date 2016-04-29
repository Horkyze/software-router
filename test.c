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
#include "headers.h"


void test_belongs_to_subnet(){
    // addr, subnet, prefix
    if ( belongs_to_subnet(string_to_ip("10.0.1.4"), string_to_ip("10.0.1.0"), 24) ){
        printf("OK\n");
    } else {
        printf("ERROR belongs_to_subnet\n");
    }

    if ( ! belongs_to_subnet(string_to_ip("10.0.1.4"), string_to_ip("10.0.2.0"), 24) ){
        printf("OK\n");
    } else {
        printf("ERROR ! belongs_to_subnet\n");
    }
}

void test_header_sizes(){
    if (sizeof(eth_2_h) == 16) { // 14 is real tho..
        printf("OK\n");
    } else {
        printf("ERROR size eth_2_h\n");
    }

    if (sizeof(udp_h) == 8) {
        printf("OK\n");
    } else {
        printf("ERROR size udp_h\n");
    }

    if (sizeof(rip_h) == 4) {
        printf("OK\n");
    } else {
        printf("ERROR size rip_h\n");
    }

    if (sizeof(rip_entry_h) == 20) {
        printf("OK\n");
    } else {
        printf("ERROR size rip_entry_h\n");
    }
}

int main (int argc, char* const argv[])
{
    test_belongs_to_subnet();
    test_header_sizes();

    return 0;
}

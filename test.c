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

#include "headers.h"
#include "functions.h"



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

void test_mask_to_prefix(){
    if (mask_to_prefix(string_to_ip("255.255.255.0")) == 24) {
        printf("OK\n");
    } else {
        printf("ERROR size mask_to_prefix \n");
    }
    if (mask_to_prefix(string_to_ip("255.255.255.255")) == 32) {
        printf("OK\n");
    } else {
        printf("ERROR size mask_to_prefix \n");
    }

    if (mask_to_prefix(string_to_ip("255.255.255.252")) == 30) {
        printf("OK\n");
    } else {
        printf("ERROR size mask_to_prefix \n");
    }

}
int main (int argc, char* const argv[])
{
    time_t t = time(0);
    printf("%lu\n", t);
    test_belongs_to_subnet();
    test_header_sizes();
    test_mask_to_prefix();

    return 0;
}

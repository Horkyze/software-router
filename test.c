#define LOG_FILE "log.txt"

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
#include "functions.h"


#include "router.h"
int main()
{
    // int i = 0;
    // for (i = 0; i < 32; i++) {
    //     printf("Netmask \\%d = %s\n", i, prefix_to_mask(i));
    // }

    // u_int ip = string_to_ip("234.0.23.0");
    // printf("IP (234.0.23.0) = %u\n", ip);
    // printf("IP (%u) = %s\n", ip, ip_to_string(ip));
    //
    //
    // printf("10.11.12.13 belongs to subnet 10.11.0.0\\16  %i\n",
    // belongs_to_subnet(string_to_ip("10.11.12.13"), string_to_ip("10.11.0.0"), 16) );
    //
    // printf("192.168.1.5 belongs to subnet 192.168.2.0\\16  %i\n",
    // belongs_to_subnet(string_to_ip("192.168.1.5"), string_to_ip("192.168.2.0"), 24) );


    //printf("Is valid IP (234.0.23.0) = %i\n", is_valid_ip("234.0.23.0"));
    //printf("Is valid IP (562.21.14.01) = %i\n", is_valid_ip("562.21.14.01"));

    char a[6] = "123456";
    char * b = (char *) malloc(6);
    printf("a: '%p' \nb: '%p'\n\n", a, b);

    printf("a: '%s' \nb: '%s'\n\n", a, b);

    strcpy(b, a);
    printf("a: '%s' \nb: '%s'\n\n", a, b);

    strcpy(b, "abcdef");
    printf("a: '%s' \nb: '%s'\n\n", a, b);

    strcpy(a, b);
    printf("a: '%s' \nb: '%s'\n\n", a, b);


	return 0;
}

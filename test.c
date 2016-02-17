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



int main()
{
    // int i = 0;
    // for (i = 0; i < 32; i++) {
    //     printf("Netmask \\%d = %s\n", i, prefix_to_mask(i));
    // }

    u_int ip = string_to_ip("234.0.23.0");
    printf("IP (234.0.23.0) = %u\n", ip);
    printf("IP (%u) = %s\n", ip, ip_to_string(ip));


    printf("10.11.12.13 belongs to subnet 10.11.0.0\\16  %i\n",
    belongs_to_subnet(string_to_ip("10.11.12.13"), string_to_ip("10.11.0.0"), 16) );

    printf("192.168.1.5 belongs to subnet 192.168.2.0\\16  %i\n",
    belongs_to_subnet(string_to_ip("192.168.1.5"), string_to_ip("192.168.2.0"), 24) );



	return 0;
}

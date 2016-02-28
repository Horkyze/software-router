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


//#pragma pack(1)
typedef struct __attribute__((packed)) arp_h {
    u_short htype;    /* Hardware Type              2 */
    u_short ptype;    /* Protocol Type              2*/
    u_char hlen;        /* Hardware Address Length  1*/
    u_char plen;        /* Protocol Address Length  1*/
    u_short oc;     /* Operation Code                2*/
    u_char __attribute__((packed)) sha[6];      /* Sender hardware address   6*/
    u_long spa;      	/* Sender IP address            4*/
    u_char __attribute__((packed)) tha[6];      /* Target hardware address    6*/
    u_long tpa;      	/* Target IP address            4 */
}arp_h;

typedef struct no_pack {
    short htype;    /* Hardware Type           */
    short ptype;    /* Protocol Type           */
    char hlen;        /* Hardware Address Length */
    char plen;        /* Protocol Address Length */
    short oc;     /* Operation Code          */
    char sha[6];      /* Sender hardware address */
    long spa;      	/* Sender IP address       */
    char tha[6];      /* Target hardware address */
    long tpa;      	/* Target IP address       */
} __attribute__((packed)) no_pack;


int main()
{

    char a[6] = "aaaaaa";
    char b[6] = "bbbbbb";
    memswap(a, b, 6);
    printf("a: '%s' \nb: '%s'\n\n", a, b);

    int i =  500;
    int j = 1000;
    memswap(&i, &j, 4);
    printf("%d, %d\n",i,j );

    printf("sizeof long  %lu\n", sizeof(u_long));
    printf("sizeof shot  %lu\n", sizeof(u_short));
    printf("sizeof char  %lu\n", sizeof(u_char));

    printf("sizeof packed  %lu\n", sizeof(arp_h));
    printf("sizeof packed  %lu\n", sizeof(arp_h));
    printf("sizeof no_pack %lu\n", sizeof(no_pack));

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





	return 0;
}

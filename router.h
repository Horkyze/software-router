/**
Software router implementation with 2 ports.
Created as a school assigment for Computer Networks curse.
Matej Bellus

**/

#define LOG_FILE "log.txt"

#pragma pack(1)

//#include "libpcap/pcap.h"
#include <pcap.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdarg.h"

#define ETH2_TYPE 2
#define IP4_TYPE 0x0008 // 0x0800 -> 0x0008 due to endianness
#define ARP_TYPE 0x0608 // 0x8060 -> 0x0608 due to endianness
#define ICMP_TYPE 0x01
#define UDP_TYPE  0x11
#define TCP_TYPE  0x06

#define R_ANY -1
#define R_ALLOW 1
#define R_DENY 0
#define R_IN 3
#define R_OUT 4
#define R_IP 5
#define R_MAC 6
//#define DEFAULT_ACTION R_DENY
#define DEFAULT_ACTION R_ALLOW
char log_b[1024];

// custom includes
#include "functions.h"
#include "stats.h"

#include "eth_parser.h"
#include "injector.h"
#include "arp.h"
#include "l3_parser.h"
#include "l4_parser.h"
#include "l5_parser.h"

pthread_mutex_t mutex;
int pause_rendering = 0;
Port *p1, *p2;

#include "routing_table.h"
//#include "rules.h"
#include "config.h"
#include "port_listener.h"

void print_usage(){
	printf("\nSoftware router implementation by Matej Bellus (c) \n");
	printf("Usage: s_router [-l] [-1 <interface_1> -2 <interface_2>] \n\n");
	printf("   -l 			List available interfaces\n");
	printf("   -1 			First interface\n");
	printf("   -2 			Second inteface\n");
	printf("\nEXAMPLE 		sudo ./bin/s_router -1 eth0 -2 wlan0\n");
}

void list_interfaces(){
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_if_t * alldevs;
	pcap_if_t * d;

	pcap_findalldevs(&alldevs, errbuf);
	for(d=alldevs; d; d = d->next){
		printf("%s\n", d->name);
  	}
}

void signal_handler(){
	my_log("Ctrl C - cya ;) ");
	//printf("\033[?1049l"); // go back
	exit(0);
}

void clear_log(){
	my_log("Clear log..");
	FILE * f;
	f = fopen(LOG_FILE, "w");
	fclose(f);
}

int router(int argc, char *argv[])
{
    //signal (SIGINT, signal_handler);
	//printf("\033[?1049h\033[H");

	clear_log();
	my_log("Software router starting...");
	my_log(pcap_lib_version());

	int option = 0, ret;
	char c;
	pthread_t config_thread;
	char errbuf[PCAP_ERRBUF_SIZE];
	p1 = create_port_struct(1);
	p2 = create_port_struct(2);

	while ((option = getopt(argc, argv,"h l 1:2: m")) != -1) {
		switch (option) {
			case '1' :
				strcpy(p1->name, optarg);
				break;
			case '2' :
				strcpy(p2->name, optarg);
				break;
			case 'l':
				list_interfaces();
				exit(0);
			case 'm':
				//mock_rule();
				break;
			case 'h':
			default: print_usage();
				exit(EXIT_FAILURE);
		}
	}

	p1->handle = pcap_create(p1->name, errbuf);
	if ( (ret = pcap_setdirection(p1->handle, PCAP_D_IN)) != 0){
		printf("pcap_setdirection returned %i\n", ret);
		my_log("pcap_setdirection failed");
		pcap_perror(p1->handle, 0);
		//exit(-1);
	}
	if ( pcap_set_promisc(p1->handle, 1) != 0){
		printf("pcap_set_promisc returned \n%s\n", pcap_geterr(p1->handle));
		my_log("pcap_set_promisc failed");
		pcap_perror(p1->handle, 0);
		exit(-1);
	}
	if ( pcap_set_immediate_mode(p1->handle, 1) != 0){
		printf("pcap_set_immediate_mode returned \n%s\n", pcap_geterr(p1->handle));
		my_log("pcap_set_immediate_mode failed");
		pcap_perror(p1->handle, 0);
		exit(-1);
	}
	if ( pcap_activate(p1->handle)){
		printf("Failed to open interface %s\n", pcap_geterr(p1->handle));
		exit(-1);
	} else {
		sprintf(log_b, "Handle activated for %s", p1->name);
		my_log(log_b);
	}




	p2->handle = pcap_create(p2->name, errbuf);
	if ( pcap_setdirection(p2->handle, PCAP_D_OUT) != 0){
		my_log("pcap_setdirection failed");
		pcap_perror(p2->handle, 0);
		//exit(-1);
	}
	if ( pcap_set_promisc(p2->handle, 1) != 0){
		my_log("pcap_set_promisc failed");
		pcap_perror(p2->handle, 0);
		exit(-1);
	}
	if ( pcap_set_immediate_mode(p2->handle, 1) != 0){
		my_log("pcap_set_immediate_mode failed");
		pcap_perror(p2->handle, 0);
		exit(-1);
	}
	if ( pcap_activate(p2->handle)){
		printf("Failed to open interface %s\n", pcap_geterr(p2->handle));
		exit(-1);
	} else {
		sprintf(log_b, "Handle activated for %s", p2->name);
		my_log(log_b);
	}

	my_log("Creating threads...");
	pthread_mutex_init(&mutex, NULL);
	if ( pthread_create(&(p1->thread), 0, port_listener, (void *)p1) ){
		my_log("Error creating p1 thread");
		exit(-1);
	}
	if ( pthread_create(&(p2->thread), 0, port_listener, (void *)p2) ){
		my_log("Error creating p2 thread");
		exit(-1);
	}

	pthread_create(&config_thread, 0, config, 0);

	while (1) {
		if(pause_rendering == 1)
			continue;

		// render here
		system("clear");
		print_routing_table();
		print_arp_cache();
		print_interface_details(p1);
		print_interface_details(p2);
		sleep(1);
	}

	pthread_join(config_thread, 0);
	pthread_join(p1->thread, 0);
	pthread_join(p2->thread, 0);

	//printf("\033[?1049l"); // go back

	return 0;
}

#ifndef HEADERS_H
#define HEADERS_H

typedef struct Stats{
	// layer 2
	u_long l2_total;
	//layer 3
	u_long l3_ip;
	u_long l3_icmp;
	u_long l3_arp;
	//layer 4
	u_long l4_tcp;
	u_long l4_udp;
	//layer 5
	u_long l5_http; // 80
	u_long l5_telnet; //2
	u_long l5_other; //2
	int dropped;
	int rip_forwarded;
	int static_forwarded;
}Stats;

typedef struct Port {
	int id;
	char * name;
	pcap_t * handle;
	pthread_t thread;
	u_int ip;
	int mask;
	u_char mac[6];
	Stats * in;
	Stats * out;
}Port;

typedef struct Route{
	u_int network;
	int mask; // prefix form
	int ad; // administrative distance
	Port * outgoing_interface; // via this interface
	time_t last_update;
}Route;

#define R ((Route *)curr->data)

typedef struct Frame {
	u_int number;
	u_int length;
	u_int parseble;
	u_int can_forward;

	void * eth_header;
	void * network_header;
	void * transport_header;
	void * data;

	int l2; // eth only
	int l3; // arp, icmp, ip
	int l4; // tcp, udp
	int l5; // containt port number

	Port * p;
	int direction;
} Frame;

Route * add_route(u_int network, int mask, Port * p, int ad);
Route * routing_table_search(u_int);

#endif

#ifndef HEADERS_H
#define HEADERS_H

// macro for referencing headers
#define EthII ((eth_2_h*)f->eth_header)
#define IPv4 ((ipv4_h*)f->network_header)
#define UDP ((udp_h*)(f->transport_header))
#define TCP ((tcp_h*)(f->transport_header))
#define ICMP ((icmp_h*)(f->transport_header))
#define BROADCAST_MAC "\xff\xff\xff\xff\xff\xff"


#define ICMP_TYPE 0x01
#define TCP_TYPE 0x06
#define UDP_TYPE 0x11 // 17

typedef	struct eth_2_h
{
	u_char dst_addr[6];
	u_char src_addr[6];
	u_short eth_type;
	u_char extra[2]; // just for parsing purposes
}eth_2_h;

typedef	struct ipv4_h
{
	u_char version_ihl;
	u_char dscp_enc;
	u_short length;
	u_short identification;
	u_short flags_fragment_offset;
	u_char ttl;
	u_char protocol;
	u_short header_chksm;
	u_int src_ip_addr;
	u_int dst_ip_addr;
	u_int options; // if ihl > 5
}ipv4_h;

// types
#define ICMP_ECHO  		0x08
#define ICMP_ECHO_REPLY 0x00

typedef struct icmp_h {
    u_char type;
    u_char code; // code is zero for ECHO
    u_short cksum;
    u_char * data;
} icmp_h;

typedef struct udp_h{
	u_short sport;
	u_short dport;
	u_short len;
	u_short chcksm;
}udp_h;


#define FIN 0x01
#define SYN 0x02
#define RST 0x04
#define PSH 0x08
#define ACK 0x10

typedef struct tcp_h{
	u_short sport;
	u_short dport;
	u_int seq_num;
	u_int ack_num;
	u_char offset;
	u_char flags;
	u_short window;
	u_short chcksm;
	u_short upointer;
	// options..
}tcp_h;


#define RIP_VERSION 0x02
#define RIP_REQUEST 0x01
#define RIP_RESPONSE 0x02
#define RIP_IP_FAMILY 0x0200 //0x0002
#define RIP_PORT 0x0802 //0x0208 520 udp
#define RIP_BROADCAST_IP 0x090000e0 //0xe0000009 224.0.0.9


typedef struct rip_h {
    u_char command;
    u_char version;
}rip_h;

typedef struct rip_entry_h {
    u_short family;
    u_short route_tag;
    u_int ip;
    u_int mask;
    u_int next_hop;
    u_int metric;
} rip_entry_h;

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
void incoming_arp(Frame *);
void incoming_icmp(Frame *);

#endif

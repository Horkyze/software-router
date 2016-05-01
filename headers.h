#ifndef HEADERS_H
#define HEADERS_H

#define FLG_CHK(a,b) ((a & b) == b)
#define FLG_ADD(a,b) ( a = a | b)
#define FLG_RM(a,b) ( a = a ^ b)

#define RIP_FLAG_INVALID 	0x00000002
#define RIP_FLAG_FLUSH   	0x00000004
#define RIP_FLAG_HOLD_DOWN 	0x00000008

#define MIN(a,b) ( (a < b) ? (a) : (b) )

#define _s16(x) __builtin_bswap16(x)
#define _s32(x) __builtin_bswap32(x)
#define _s64(x) __builtin_bswap64(x)

// macro for referencing headers
#define EthII ((eth_2_h*)f->eth_header)
#define IPv4 ((ipv4_h*)f->network_header)
#define UDP ((udp_h*)(f->transport_header))
#define TCP ((tcp_h*)(f->transport_header))
#define ICMP ((icmp_h*)(f->transport_header))
#define RIP_H ((rip_h*)(f->app_layer))
#define RIP_E(x) ((rip_entry_h*)(f->app_layer+4+(x*20)))
#define RIP_OFFSET_E(ptr, x) ((rip_entry_h*)(ptr+(x*20)))

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

struct pseudo_header {
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t udp_length;
};


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
#define RIP_MULTICAST_MAC "\x01\x00\x5e\x00\x00\x09" //01-00-5E-00-00-00
#define RIP_INFINITY 16


typedef struct rip_h {
    u_char command;
    u_char version;
	u_short zeros;
}rip_h;

typedef struct rip_entry_h {
    u_short family;
    u_short route_tag;
    u_int ip;
    u_int mask;
    u_int next_hop;
    u_int metric; // hop count
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

#define RIP_AD 110
#define STATIC_AD 1
#define DIRECTLY_CONNECTED_AD 0
// entries marked with this flag were set by
// rip <network> command
#define RIP_FLAG_DB 0x00000001
typedef struct Route{
	u_int network;
	int mask; // prefix form
	int ad; // administrative distance
	Port * outgoing_interface; // via this interface
	u_int flags;
	u_int metric; // hop count
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
	void * app_layer;

	void * data;

	int l2; // eth only
	int l3; // arp, icmp, ip
	int l4; // tcp, udp
	int l5; // containt port number

	Port * p;
	int direction;
} Frame;

char * get_route(Route * r);
Route * add_route(u_int network, int mask, Port * p, int ad, u_int flags);
Route * routing_table_search(u_int);
int routing_table_delete(Route * r, int id);

void incoming_arp(Frame *);
void incoming_icmp(Frame *);
void incoming_rip(Frame *);

Frame * RIP_generate_update(Port *);

#endif

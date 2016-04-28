#define RIP_VERSION 0x02
#define RIP_REQUEST 0x01
#define RIP_RESPONSE 0x02
#define RIP_IP_FAMILY 0x0200 //0x0002
#define RIP_PORT 0x0802 //0x0208 520 udp
#define RIP_BROADCAST_IP 0x090000e0 //0xe0000009 224.0.0.9


typedef struct rip_h {
    u_char command;
    u_char version;
}

typedef struct rip_entry_h {
    u_short family;
    u_short route_tag;
    u_int ip;
    u_int mask;
    u_int next_hop;
    u_int metric;
}

void incoming_rip(Frame * f){

}

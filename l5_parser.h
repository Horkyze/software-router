#define UDP ((udp_h*)(f->transport_header))
#define TCP ((tcp_h*)(f->transport_header))

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


void parse_l5(Frame * f){
    if (f->l4 == UDP_TYPE) {
        f->l5 = UDP->sport;
    } else if (f->l4 == TCP_TYPE) {
        f->l5 = TCP->sport;
    }


	if (f->l5){
		// the end
	} else {
		//my_log("Failed to parse L5, header not supported");
	}
}

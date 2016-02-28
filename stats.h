
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

typedef struct Frame {
	u_int number;
	u_int length;
	u_int parseble;

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

Stats * create_stats_struct(){
	Stats * s;
	s = (Stats *) calloc(sizeof(Stats), 1);
	return s;
}

Port * create_port_struct(int i){
	Port * p;
	p = (Port *) malloc(sizeof(Port));
	p->id = i;
	p->name = (char *) calloc(1, 100);
	p->handle = 0;
	p->thread = 0;
	p->in = create_stats_struct();
	p->out = create_stats_struct();
	p->ip = string_to_ip("10.0.0.1");
	p->mask = 24;
	strcpy((char *)p->mac, "\xac\xbc\x32\xb9\x1b\xb3");
	return p;
}

void update_stats(Frame * f, Port * p, int direction){
	Stats * s = (direction == R_IN)? p->in : p->out;

	if (f->l2 == ETH2_TYPE) {
		s->l2_total++;
	} else {
		return;
	}

	if (f->l3 == IP4_TYPE) {
		s->l3_ip++;
	} else if (f->l3 == ARP_TYPE) {
		s->l3_arp++;
		return;
	}
	if (f->l4 == ICMP_TYPE) {
		s->l3_icmp++;
		return;
	}

	if (f->l5 == 80) {
		s->l5_http++;
	} else if (f->l5 == 23) {
		s->l5_telnet++;
	} else {
		s->l5_other++;
	}
}

void print_stats_header(){
	printf("\nPort\tTotal\tIP\tICMP\tARP\tTCP\tUDP\tHTTP\tTelnet\tOther\n");
	printf("-----------------------------------------------------------------------------\n");
}

void print_stats(Stats * s, const char * port){
	printf("%s\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t \n",
		port, s->l2_total, s->l3_ip, s->l3_icmp, s->l3_arp, s->l4_tcp, s->l4_udp, s->l5_http, s->l5_telnet, s->l5_other);
}

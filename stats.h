Frame * new_frame(){
	Frame * f = (Frame * ) calloc(1, sizeof(Frame));
	f->length = 60;
	f->eth_header = calloc(1, f->length); // minimal eth size
    f->network_header = (char *)f->eth_header + 0x0e; // size of eth_II 14
	return f;
}

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
	if (i == 1) {
		p->ip = string_to_ip("176.16.0.2");
		p->mask = 30;
		strcpy((char *)p->mac, "\x0A\x01\x01\x01\x01\x01");
	} else if (i == 2) {
		p->ip = string_to_ip("10.0.0.1");
		p->mask = 25;
		strcpy((char *)p->mac, "\x0A\x02\x02\x02\x02\x02");
	}
	add_route(ip_mask_to_network(p->ip, p->mask), p->mask, p, 0, 0);

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

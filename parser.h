const u_char eth_max[] = {0x06, 0x00}; // 1536

char * get_src_mac(Frame * f){
	eth_2_h * hdr;
	hdr = (eth_2_h*)f->eth_header;
	return get_hex(hdr->src_addr, 6, ':');
}
char * get_dst_mac(Frame * f){
	eth_2_h * hdr;
	hdr = (eth_2_h*)f->eth_header;
	return get_hex(hdr->dst_addr, 6, ':');
}
int is_broadcast(Frame * f){
	char * c;
	c = get_dst_mac(f);
	if (strcasecmp(c, "ff:ff:ff:ff:ff:ff") == 0) {
		return 1;
	}
	return 0;
}

int is_for_me_eth(Frame * f){
	if (is_broadcast(f)) {
		return 1;
	}
	if( memcmp(EthII->dst_addr, &f->p->mac, 6) == 0){
		return 1;
	}
	return 0;
}

void parse_l2(Frame * f);
void parse_l3(Frame * f);
void parse_l4(Frame * f);
void parse_l5(Frame * f);

void parse_l2(Frame * f) {
	if( EthII->eth_type >= 0x0006) {
		// we got eth2
		f->l2 = ETH2_TYPE;
		if (is_for_me_eth(f)) {
			parse_l3(f);
		} else {
			my_log("[PARSER] \tFrame is not for me..");
		}
	} else {
		my_log("[PARSER] \tFailed to parse L2, header not supported");
	}
}

void parse_l3(Frame * f) {
	f->network_header = (char *)f->eth_header + 14;
	if( EthII->eth_type == IP4_TYPE){

		f->l3 = IP4_TYPE;
		f->can_forward = 1;
		parse_l4(f);
	} else if( EthII->eth_type == ARP_TYPE){
		f->l3 = ARP_TYPE;
		f->can_forward = 0;
		incoming_arp(f);
	} else {
		my_log("[PARSER] \tFailed to parse L3, header not supported");
	}
}

void parse_udp(Frame * f){

}

void parse_l4(Frame * f){

	if (IPv4->protocol == ICMP_TYPE){
		f->l4 = ICMP_TYPE;
		incoming_icmp(f); // let icmp decide what to do
	} else if (IPv4->protocol == TCP_TYPE){
		f->l4 = TCP_TYPE;
		f->can_forward = 1; // will be forwarded automatically
	} else if (IPv4->protocol == UDP_TYPE){
		f->l4 = UDP_TYPE;
		parse_udp(f); // frame is maybe RIP, let udp decide
	} else {
		my_log("[PARSER] \tFailed to parse L4, header not supported");
		f->can_forward = 1; // just forward
	}
}

Frame * add_frame(u_char * data, int length, Port * p, int d){
	struct eth_2_h * hdr = (eth_2_h *) data;
    Frame * frame = (Frame *) calloc(sizeof(Frame), 1);
	frame->length = length;
	frame->data = malloc(length);
	frame->eth_header = frame->data;
	frame->parseble = 0;
	frame->p = p;
	frame->direction = d;
	frame->can_forward = 0;
	int k = memcmp(&(hdr->eth_type), eth_max, 2);
	// if we have ETH II
	if ( k >= 0){
		frame->parseble = 1;
	}
	memcpy ( frame->data, data, length);
	parse_l2(frame);
    return frame;
}

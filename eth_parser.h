const u_char eth_max[] = {0x06, 0x00}; // 1536

// macro for referencing eth II header
#define EthII ((eth_2_h*)f->eth_header)

/*==========  Ethernet Headers Definitions  ==========*/
typedef	struct eth_2_h
{
	u_char dst_addr[6];
	u_char src_addr[6];
	u_short eth_type;
	u_char extra[2]; // just for parsing purposes
}eth_2_h;

typedef	struct eth_raw_h
{
	u_char dst_addr[6];
	u_char src_addr[6];
	u_char length[2];
	u_char ipx_hdr[3];

}eth_raw_h;

typedef	struct eth_llc_h
{
	u_char dst_addr[6];
	u_char src_addr[6];
	u_char length[2];
	u_char dsap[1];
	u_char ssap[1];
	u_char control[1];
}eth_llc_h;


typedef	struct eth_llc_snap_h
{
	u_char dst_addr[6];
	u_char src_addr[6];
	u_char length[2];
	u_char dsap[1];
	u_char ssap[1];
	u_char control[1];
	u_char vendor[3];
	u_char eth_type[2];
}eth_llc_snap_h;


/*-----  End of Ethernet Header Declaration  ------*/



void parse_l2(Frame * f);
void parse_l3(Frame * f);
void parse_l4(Frame * f);
void parse_l5(Frame * f);


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
void parse_l2(Frame * f){
	if( EthII->eth_type >= 0x0006) {
		// we got eth2
		f->l2 = ETH2_TYPE;
	}
	// add next l2 protocols..

	if (f->l2){
		parse_l3(f);
	} else {
		my_log("Failed to parse L2, header not supported");
	}

}

void print_frame(Frame * f){
	sprintf(log_b, "L2: %i\tL3: %i\tL4: %i\tL5: %i\t ",
	f->l2, f->l3, f->l4, f->l5);
	sprintf(log_b, "\tsrc: %s, dst: %s", get_src_mac(f), get_dst_mac(f));
	my_log(log_b);
}


Frame * add_frame(u_char * data, int length, Port * p, int d){

	struct eth_2_h * hdr = (eth_2_h *) data;

	Frame * frame = calloc(sizeof(Frame), 1);
	frame->length = length;
	frame->data = malloc(length);
	frame->eth_header = frame->data;
	frame->parseble = 0;
	frame->p = p;
	frame->direction = d;
	int k = memcmp(&(hdr->eth_type), eth_max, 2);
	// if we have ETH II
	if ( k >= 0){
		frame->parseble = 1;
	}

	memcpy ( frame->data, data, length);

	parse_l2(frame);

    return frame;
}

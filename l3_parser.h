// macro for referencing eth II header
#define EthII ((eth_2_h*)f->eth_header)



// arp, ip, ospf, eigrp, ???
void parse_l3(Frame * f){

	if (f->l2 == ETH2_TYPE){
        f->network_header = (char *)f->eth_header + 14;

		if( EthII->eth_type == IP4_TYPE){
			f->l3 = IP4_TYPE;
			f->can_forward = 1;
		}
		if( EthII->eth_type == ARP_TYPE){
			f->l3 = ARP_TYPE;
			f->can_forward = 1;
			incoming_arp(f);
		}
	}

	if (f->l3){
		parse_l4(f);
	} else {
		//my_log("Failed to parse L3, header not supported");
	}
}

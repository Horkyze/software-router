// macro for referencing ipv4 header
#define IPv4 ((ipv4_h*)f->network_header)



typedef	struct ipv4_h {
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

char * get_src_ip(Frame * f){
	return ip_to_string(IPv4->src_ip_addr);
}
char * get_dst_ip(Frame * f){
	return ip_to_string(IPv4->dst_ip_addr);
}

void parse_l4(Frame * f){
    if (f->l3 == IP4_TYPE) {
        f->transport_header = f->network_header + 20;

    	if (IPv4->protocol == ICMP_TYPE){
    		f->l4 = ICMP_TYPE;
    	} else if (IPv4->protocol == TCP_TYPE){
    		f->l4 = TCP_TYPE;
    	} else if (IPv4->protocol == UDP_TYPE){
    		f->l4 = UDP_TYPE;
    	}
    }
	if (f->l4){
		parse_l5(f);
	} else {
		//my_log("Failed to parse L4, header not supported");
	}
}

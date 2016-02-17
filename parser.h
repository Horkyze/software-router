
/*===================================================
=            Ethernet Header Declaration            =
===================================================*/

typedef	struct eth_2_h
{
	u_char dst_addr[6];
	u_char src_addr[6];
	u_char eth_type[2];
	u_char extra[2]; // just for parsing purposes
}Eth_II_hdr;

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

const u_char eth_max[] = {0x06, 0x00}; // 1536 

typedef	struct arp_h
{
	u_char hat[2];
	u_char pat[2];
	u_char hw_addr_len[1];
	u_char proto_addr_len[1];
	u_char operations[2];
	u_char src_hw_addr[6];
	u_char src_proto_addr[4];
	u_char target_hw_addr[6];
	u_char target_proto_addr[4];

}arp_h;

typedef	struct ipv4_h
{
	u_char version_ihl[1];
	u_char dscp_enc[1];
	u_char length[2];
	u_char identification[2];
	u_char flags_fragment_offset[2];
	u_char ttl[1];
	u_char protocol[1];
	u_char header_chksm[2];
	u_char src_ip_addr[4];
	u_char dst_ip_addr[4];
	u_char options[4]; // if ihl > 5

}ipv4_h;





void parse_eth(u_char * data){
	struct eth_2_h * hdr = data;

	printf("Dst Addr : "); 
	print(hdr->dst_addr, 6);

	printf("Src Addr : "); 
	print(hdr->src_addr, 6);

	printf("Length / type : "); 
	print(hdr->eth_type, 2);


	int k = memcmp(hdr->eth_type, eth_max, 2);
	printf("cmp: %i\n", k);
	if ( k >= 0){
		
		printf("Ethernet II\n");

		if ( memcmp(hdr->eth_type, arp, 2) == 0 ){
			printf("  Got ARP packet!\n");
		}

		if ( memcmp(hdr->eth_type, ipv4, 2) == 0 ){
			printf("  Got IPv4 packet!\n");
			ipv4_h * ip = data[14];
			printf("Source IP: ");
			print_ip(&ip->src_ip_addr);
			printf("\n");
		}

	
	} else {
		// dont perform more analysis than this 
		if (hdr->extra[0] == 0xFF && hdr->extra[1] == 0xFF)
		{
			printf("Ethernet RAW\n");
		} else if (hdr->extra[0] == 0xAA && hdr->extra[1] == 0xAA)
		{
			printf("Ethernet LLC/SNAP\n");
		} else {
			printf("Ethernet LLC\n");
		}
	}



}
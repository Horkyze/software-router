#define RIP_T_UPDATE 5
#define RIP_T_INVALID 180
#define RIP_T_FLUSH 240
//#define RIP_T_HOLD_DOWN 180

void RIP_flush() {

}

void RIP_invalid() {

}

void RIP_update() {
    // send periodic updates
    // for each interface
    my_log("[RIP] \t   Update timer");
    inject_frame(RIP_generate_update(p1), p1);
    inject_frame(RIP_generate_update(p2), p2);
}


// thread main func
void * rip_timers(void * arg){
    time_t start_time = time(0);
    time_t now;
    while (1) {
        now = time(0);
        if ( ( now - start_time ) % RIP_T_FLUSH == 0) {
            RIP_flush();
        }
        if ( ( now - start_time ) % RIP_T_INVALID == 0) {
            RIP_invalid();
        }
        if ( ( now - start_time ) % RIP_T_UPDATE == 0) {
            RIP_update();
        }
        sleep(3);
    }
}



Frame * RIP_generate_update(Port * p){
    // send all rip prefixes except p->ip
    if (routes_ll == 0){
		return 0;
	}

    long hdrs_size = 14 + 20 + 8 + 4;
    long total_size = hdrs_size;
    //add rip RIP_FLAG_DB enabled routes_ll
    rip_entry_h * rip_entries = 0;
	// sprintf(log_b, "[R_TABLE]\t routing_table_search: %s", ip_to_string(dst));
	// my_log(log_b);

	Item * curr = (Item *) routes_ll->head;
    int i;
	for(i = 0; curr; ){
		if ( FLG_CHK( R->flags, RIP_FLAG_DB ) ){
			my_log("[RIP]\t Found route with flag");
            total_size += sizeof(rip_entry_h);
            rip_entries = (rip_entry_h *) malloc(sizeof(rip_entry_h));//realloc(rip_entries, 20 );

            // prepare rip_entry_h
            rip_entries->family    = RIP_IP_FAMILY;
            rip_entries->route_tag = 0x0000;
            rip_entries->ip        = R->network;
            rip_entries->mask      = htonl(prefix_to_mask(R->mask));
            rip_entries->next_hop  = 0x00000000;
            rip_entries->metric    = htonl(1);
            i++;
		}
		curr = curr->next;
	}

    if (hdrs_size == total_size) { // if NO routes were found
        return 0; // do not generate frame
    }

    u_int data_size = total_size - hdrs_size + 4; // whole rip stuff

    // generate frame here
    Frame * f = new_frame();
    f->eth_header = malloc(total_size);
    f->network_header = (char *)f->eth_header + 0x0e;
    f->transport_header = f->network_header + 20;
    f->app_layer = f->transport_header + 8;

    // eth
    memcpy(EthII->dst_addr , RIP_MULTICAST_MAC, 6); // should calculate special mac?
    memcpy(EthII->src_addr , p->mac, 6);
    EthII->eth_type = IP4_TYPE; // maybe _s32() ??

    //ip
    IPv4->version_ihl = 0x45;
    IPv4->dscp_enc = 0xc0;
    IPv4->length = htons(20 + 8 + data_size); // ip + udp + rip
    IPv4->identification = 0x0000;
    IPv4->flags_fragment_offset = 0x0000 ;
    IPv4->ttl = 0x02;
    IPv4->protocol = UDP_TYPE;
    IPv4->header_chksm = 0;
    IPv4->src_ip_addr = p->ip;
    IPv4->dst_ip_addr = RIP_BROADCAST_IP;

    IPv4->header_chksm = csum ((u_short *) f->network_header, 20);

    //udp
    UDP->sport = RIP_PORT;
    UDP->dport = RIP_PORT;
    UDP->len = htons(8 + data_size); //calculate
    UDP->chcksm = 0; //calculate

    // RIP_H
    RIP_H->command = RIP_RESPONSE;
    RIP_H->version = RIP_VERSION;
    RIP_H->zeros = 0x0000;

    // copy rip entries
    memcpy(f->app_layer + 4, rip_entries, sizeof(rip_entry_h));

    //calculate checksum
    struct pseudo_header psh;
    char * pseudogram;

    psh.source_address = IPv4->src_ip_addr;
    psh.dest_address = IPv4->dst_ip_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_UDP;
    psh.udp_length = UDP->len;

    int psize = sizeof(struct pseudo_header) + 8 + data_size;
    pseudogram = (char *) malloc(psize);

    memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header) , f->transport_header , 8 + data_size);

    UDP->chcksm = csum( (u_short *) pseudogram , psize);

    // fill frame params
    f->data = f->eth_header;
    f->length = total_size;

    return f;
}

int number_of_rip_entries(Frame * f){
    u_short total = _s16(UDP->len) - sizeof(udp_h) - sizeof(rip_h);
    u_short one = sizeof(rip_entry_h);
    if (total % one != 0){
        my_log("[RIP-DEBUG] \t WTF !!!!!! Number of rip entries is float..");
        return -1;
    }
    return (int) total / one;
}

void debug_rip(Frame * f){
    int total = number_of_rip_entries(f);
    int i = 1;
    sprintf(log_b, "[RIP-DEBUG] \t n. of entries = %i", total);
    my_log(log_b);
    sprintf(log_b, "[RIP-DEBUG] \t command       = %i", RIP_H->command);
    my_log(log_b);
    for (i = 0; total-- && total > -1; i++){
        sprintf(log_b, "[RIP-DEBUG] \t ENTRY  %i", i);
        my_log(log_b);
        sprintf(log_b, "[RIP-DEBUG] \t %s %s", ip_to_string(RIP_E(i)->ip), ip_to_string(RIP_E(i)->mask) );
        my_log(log_b);
    }
}

void RIP_parse_entry(Frame * f, int i){
    Route * r = routing_table_search(RIP_E(i)->ip);
    if(r){
        my_log("lolo");
    } else {
        my_log("lolo not found");

        // add new Route
        add_route(RIP_E(i)->ip, mask_to_prefix(RIP_E(i)->mask), f->p, RIP_AD, 0);
    }
}

void incoming_rip(Frame * f){
    debug_rip(f);
    if(RIP_H->command == RIP_RESPONSE){

        int n = number_of_rip_entries(f);
        while (n-- && n > -1){
            RIP_parse_entry(f, n);
        }
    } else { // RIP_REQUEST

    }
}

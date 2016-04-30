
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
        add_route(RIP_E(i)->ip, mask_to_prefix(RIP_E(i)->mask), f->p, RIP_AD);
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

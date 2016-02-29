#define ARP(x) ((arp_h*)((x)->network_header))
#define A ((Arp *)curr->data)

#define ARP_TIMEOUT 5
#define ARP_REQUEST 0x0100   // ARP Request
#define ARP_REPLY 	0x0200   // ARP Reply

#pragma pack(1)
typedef struct  __attribute__((packed)) arp_h {
    u_short htype;    /* Hardware Type           */
    u_short ptype;    /* Protocol Type           */
    u_char hlen;        /* Hardware Address Length */
    u_char plen;        /* Protocol Address Length */
    u_short oc;     /* Operation Code          */
    u_char sha[6];      /* Sender hardware address */
    u_int spa;      	/* Sender IP address       */
    u_char tha[6];      /* Target hardware address */
    u_int tpa;      	/* Target IP address       */
}arp_h;


void print_arp(Frame * f){
  printf("%x\n", ARP(f)->htype );
  printf("%x\n", ARP(f)->ptype );
  printf("%x\n", ARP(f)->hlen );
  printf("%x\n", ARP(f)->plen );
  printf("%x\n", ARP(f)->oc );
  printf("%s\n", get_hex(ARP(f)->sha, 6, ':'));
  printf("%x\n", ARP(f)->spa );
  printf("%s\n", get_hex(ARP(f)->tha, 6, ':'));
  printf("%x\n", ARP(f)->tpa );
}

LL * arp_cache_ll = 0;

typedef struct Arp{
	u_int ip;
    u_char mac[6];
	Port * outgoing_interface; // via this interface
	time_t last_update;
}Arp;

Arp * add_arp(u_long ip, u_char * mac){
    Arp * a = 0;
    a = (Arp *) malloc(sizeof(Arp));
    a->ip = ip;
    memcpy(a->mac , mac, 6);
    a->last_update = time(0);
    // what about interface???
    return a;
}

u_char * arp_cache_search(u_long ip){
    if (arp_cache_ll == 0) {
        arp_cache_ll = LL_init();
        return 0;
    }
    Item * curr = (Item *) arp_cache_ll->head;
    while(curr){
        if (A->ip == ip) {
            return A->mac;
        }
        curr = curr->next;
    }
    return 0;
}

void arp_cache_update(u_long ip, u_char * mac){
    if (arp_cache_ll == 0) {
        arp_cache_ll = LL_init();
    }
    int update_flag = 0;
    Item * curr = (Item *) arp_cache_ll->head;
    while(curr){
        if (A->ip == ip) {
            memcpy(A->mac , mac, 6);
            update_flag = 1;
        }
        curr = curr->next;
    }
    if (! update_flag) {
        LL_add(arp_cache_ll, add_arp(ip, mac));
    }
}

// Merge_flag := false
//     If the pair <protocol type, sender protocol address> is
//         already in my translation table, update the sender
//         hardware address field of the entry with the new
//         information in the packet and set Merge_flag to true.
//     ?Am I the target protocol address?
//     Yes:
//       If Merge_flag is false, add the triplet <protocol type,
//           sender protocol address, sender hardware address> to
//           the translation table.
//       ?Is the opcode ares_op$REQUEST?  (NOW look at the opcode!!)
//       Yes:
//         Swap hardware and protocol fields, putting the local
//             hardware and protocol addresses in the sender fields.
//         Set the ar$op field to ares_op$REPLY
//         Send the packet to the (new) target hardware address on
//             the same hardware on which the request was received.

void incoming_arp(Frame * f){
    int merge_flag = 0;
    my_log("incoming_arp");
    if (arp_cache_search(ARP(f)->spa)) {
        arp_cache_update(ARP(f)->spa, ARP(f)->sha);
        merge_flag = 1;
        my_log("found entry");
    }

    if( ARP(f)->tpa == f->p->ip || ARP(f)->tpa == 0){ // is the packet for me?
        my_log("for me");

        if(merge_flag == 0){
            arp_cache_update(ARP(f)->spa, ARP(f)->sha); //add to table
        }
        if (ARP(f)->oc == ARP_REQUEST) {
            // swap..
            memswap(EthII->dst_addr , EthII->src_addr, 6);
            // mac + ip size = 6 bytes + 4 bytes = 10 bytes
            memswap(ARP(f)->sha, ARP(f)->tha, 10);
            memcpy(ARP(f)->sha, f->p->mac, 6 );
            ARP(f)->spa = f->p->ip;
            ARP(f)->oc = ARP_REPLY;
            inject_frame(f, f->p);
            // inject packet
        }
    } else {
        my_log("not for me");

    }


}

u_char * arp_get(u_int ip, Port * out){
    u_char * mac = 0;
    mac = arp_cache_search(ip);
    if (mac) {
        return mac;
    } else {
        Frame * f = new_frame();
        memcpy(EthII->src_addr, out->mac, 6);
        memcpy(EthII->dst_addr, BROADCAST_MAC, 6);
        EthII->eth_type = ARP_TYPE;
        ARP(f)->htype = 0x0100;
        ARP(f)->ptype = IP4_TYPE;
        ARP(f)->hlen = 6;
        ARP(f)->plen = 4;
        ARP(f)->oc = ARP_REQUEST;
        strcpy((char *)ARP(f)->sha, (char *)out->mac);
        ARP(f)->spa = out->ip;
        //ARP(f)->tha = 00:00:00:00:00:00;
        ARP(f)->tpa = ip;

        inject_frame(f, out);

        time_t send_time = time(0);
        while(1){
            mac = arp_cache_search(ip);
            if (time(0) - send_time > ARP_TIMEOUT) {
                my_log("ERROR !!! did not recieve arp reply in given timeout");
                break;
            }
            if (mac){
                return mac;
            }

        }
        return 0;



    }
}


void print_arp_cache(){
	printf("-------- ARP CACHE --------\n");
	if (arp_cache_ll == 0){
		printf("--------    END    --------\n");
		return;
	}
	Item * curr = (Item *) arp_cache_ll->head;

	while(curr){
        printf("%s \tis at %s\n", ip_to_string(A->ip), get_hex(A->mac, 6, ':') );
        curr = curr->next;
	}
	printf("--------    END    --------\n");
}

void clear_arp_cache(){
	// for each bla bla..
	free(arp_cache_ll);
	arp_cache_ll = 0;
}

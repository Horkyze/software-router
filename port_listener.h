#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int forward_packet(Route * route, Frame * f) {
	if (f->can_forward == 0) {
		return 0;
	}

	u_char * mac = arp_get(IPv4->dst_ip_addr, route->outgoing_interface);

	if ( mac ) {
		my_log("[ROUTER] \t forwarding frame");
		memcpy(EthII->src_addr , route->outgoing_interface->mac, 6 );
		memcpy(EthII->dst_addr , mac, 6 );
		inject_frame(f, route->outgoing_interface);
	}



	return 0;
}

void * port_listener(void * arg) {
	Frame * f;
	Port * p, * tmp;
	p = (Port *)arg;
	Route * route = 0;

	struct pcap_pkthdr * header;
	const u_char * packet;
	int forward, check, filter_check;

	sprintf(log_b, "Hello from thread on %s", p->name);
	my_log(log_b);

	while (1) {
		check = pcap_next_ex(p->handle, &header, &packet);
		//pthread_mutex_lock(&mutex);
		if (check) {

			my_log("\n");
			sprintf(log_b, "-- > Port %i (%s)\tgot %i bytes",
			p->id, p->name, header->len);
			my_log(log_b);

			// more like parse frame..
			f = add_frame((u_char*)packet, header->len, p, R_IN);
			//print_frame(f);

			// check INBOUNT rules
			// filter_check = apply_rules(f, p, R_IN);
			// if (filter_check) {
			//
			// }
			if (f->can_forward) {
				route = routing_table_search(IPv4->dst_ip_addr);
			} else {
				//my_log("cant forward");

				// this is not an Error
				// incoming_arp() or incoming_icmp may respond
			}

			if(route){
				forward_packet(route, f);
			} else if (f->can_forward){
				sprintf(log_b, "no route found for ip: %s", ip_to_string(IPv4->dst_ip_addr));
				my_log(log_b);
				//print_frame(f);
				// default route??
				// drop ??
			}

		} else {
			sprintf(log_b, "Failed to get frame: pcpa_next_ex returned: %i", check);
			my_log(log_b);
		}
		//pthread_mutex_unlock (&mutex);
	}

	pcap_close(p->handle);
	return 0;
}

void print_interface_details(Port * p){
	printf("Interface p%i (%s): %s\\%i %s\n", p->id, p->name, ip_to_string(p->ip), p->mask, get_hex(p->mac, 6, ':'));
}

char * get_interface_detail(Port * p){
	char * s = malloc(1024);
	sprintf(s, "Interface p%i (%s): %s\\%i %s\n", p->id, p->name, ip_to_string(p->ip), p->mask, get_hex(p->mac, 6, ':'));
	return s;
}

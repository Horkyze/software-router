#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int forward_packet(Port * interface, Frame * f) {
	// if (!apply_rules(f, ppp, R_OUT)) {
	// 	sprintf(log_b, "\tPort %i (%s) OUTbound rules blocked this frame", ppp->id, ppp->name);
	// 	my_log(log_b);
	// 	return 0;
	// }
	update_source_mac(f);
	// destination mac via arp
	// arp -i en0 192.168.1.1 | cut -d " " -f4
	// next hop ??
	
	int sent_bytes = pcap_inject(interface->handle, f->eth_header, f->length);
	if (sent_bytes == f->length) {
		//update_stats(f, interface, R_OUT);
		sprintf(log_b, "\t< -- Port %i (%s) sending %i bytes", interface->id, interface->name, sent_bytes);
		my_log(log_b);
	} else {
		my_log("Failed to send frame with specific size :(");
	}
	return sent_bytes;
}

void * port_listener(void * arg) {
	Frame * f;
	Port * p, * tmp;
	p = (Port *)arg;
	Route * route;

	struct pcap_pkthdr * header;
	const u_char * packet;
	int forward, check, filter_check;

	sprintf(log_b, "Hello from thread on %s", p->name);
	my_log(log_b);

	while (1) {
		check = pcap_next_ex(p->handle, &header, &packet);
		pthread_mutex_lock(&mutex);
		if (check) {

			sprintf(log_b, "-- > Port %i (%s)\tgot %i bytes",
			p->id, p->name, header->len);
			//my_log(log_b);

			// more like parse frame..
			f = add_frame((u_char*)packet, header->len, p, R_IN);
			//print_frame(f);

			// check INBOUNT rules
			// filter_check = apply_rules(f, p, R_IN);
			// if (filter_check) {
			//
			// }

			route = routing_table_search(IPv4->src_ip_addr);
			if(route){
				forward_packet(route, f);
			} else {
				// default route??
				// drop ??
			}


		} else {
			sprintf(log_b, "Failed to get frame: pcpa_next_ex returned: %i", check);
			my_log(log_b);
		}
		pthread_mutex_unlock (&mutex);
	}

	pcap_close(p->handle);
	return 0;
}

void print_interface_details(Port * p){
	printf("Interface p%i (%s): %s\\%i\n", p->id, p->name, ip_to_string(p->ip), p->mask);
}

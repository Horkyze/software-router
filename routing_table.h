
#define TABLE "table.txt"
#define IP "174.234.132.154"
#define MAX 1024
#define IP_SIZE 15 // 123.456.789.000

Route * add_route(u_int network, int mask, Port * p, int ad, u_int flags){
	sprintf(log_b, "[R_TABLE] \t ADD %s\\%i port: %s  ad: %i", ip_to_string(network), mask, p->name, ad );
	my_log(log_b);
	if(routes_ll == 0){
		routes_ll = LL_init();
	}
	Route * r;
	r = (Route *) malloc(sizeof(Route));
	r->network = network;
	r->mask = mask;
	r->flags = flags;
	r->ad = ad;
	r->outgoing_interface = p;
	time (&r->last_update);
	LL_add(routes_ll, r);
	return r;
}

void print_routing_table(){
	printf("-------- ROUTING TABLE --------\n");
	if (routes_ll == 0){
		printf("--------    END    --------\n");
		return;
	}
	Item * curr = (Item *) routes_ll->head;

	while(curr){
		printf("Network %s \t\\%u via %s, %s\t %s\n", ip_to_string(R->network), R->mask, R->outgoing_interface->name,
		(R->ad == RIP_AD)? "RIP" : (R->ad == STATIC_AD)? "STATIC" : "CONNECTED",
	 	get_time(&R->last_update));
		curr = curr->next;
	}
	printf("--------    END    --------\n");
}

void clear_routing_table(){
	// for each bla bla..
	free(routes_ll);
	routes_ll = 0;
}

// void mac_delete_old_entries(int older_than){
// 	if (routes_ll == 0){
// 		return;
// 	}
// 	time_t t;
// 	t = time(0);
// 	int i = 0;
// 	Item * curr = (Item *) routes_ll->head;
// 	while(curr){
// 		if ( t - ((Route *)curr->data)->last_update > older_than ){
// 			// sprintf(log_b, "AUTO deleted entry from mac table (%s)", ((Route *)curr->data)->mac );
// 			// my_log(log_b);
// 			LL_delete(routes_ll, i);
// 		}
// 		curr = curr->next;
// 		i++;
// 	}
// 	return;
//}



Route * routing_table_search(u_int dst){
	if (routes_ll == 0){
		return 0;
	}
	sprintf(log_b, "[R_TABLE]\t routing_table_search: %s", ip_to_string(dst));
	my_log(log_b);
	Item * curr = (Item *) routes_ll->head;
	while(curr){
		if ( belongs_to_subnet(dst, R->network, R->mask) ){
			my_log("[R_TABLE]\t Found ");
			return R;
		} else {
			sprintf(log_b, "[R_TABLE]\t failed belongs_to_subnet: %s %s %i", ip_to_string(dst), ip_to_string(R->network), R->mask);
			my_log(log_b);
		}
		curr = curr->next;
	}
	my_log("[R_TABLE]\t NOT found");
	return 0;
}

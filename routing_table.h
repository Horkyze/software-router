
#define TABLE "table.txt"
#define IP "174.234.132.154"
#define MAX 1024
#define IP_SIZE 15 // 123.456.789.000

#define RIP_AD 110
#define STATIC_AD 1

#define R ((Route *)curr->data)

LL * routes_ll = 0;

typedef struct Route{
	u_int network;
	int mask; // prefix form
	int ad; // administrative distance
	Port * outgoing_interface; // via this interface
	time_t last_update;
}Route;

Route * add_route(u_int network, int mask, Port * p, int ad, time_t t){
	if(routes_ll == 0){
		routes_ll = LL_init();
	}
	Route * r;
	r = (Route *) malloc(sizeof(Route));
	r->network = network;
	r->mask = mask;
	r->ad = ad;
	r->outgoing_interface = p;
	r->last_update = t;
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
		printf("Network %s\\%u via %s, %s\n", ip_to_string(R->network), R->mask, R->outgoing_interface->name, (R->ad == RIP_AD)? "RIP" : "STATIC");
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

	Item * curr = (Item *) routes_ll->head;
	while(curr){
		if ( belongs_to_subnet(dst, R->network, R->mask) ){
			return R;
		}
		curr = curr->next;
	}
	return 0;
}

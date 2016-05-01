
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
	r->metric = 0; // only RIP
	r->outgoing_interface = p;
	time (&r->last_update);
	LL_add(routes_ll, r);
	return r;
}

char * get_route(Route * r){
	char * s = (char *) calloc(1024, 1);
	sprintf(s, "Network %15s\t\\%u via %s, %s\t %s %s",
		ip_to_string(r->network), r->mask, r->outgoing_interface->name,
		(r->ad == RIP_AD)? "RIP      " : (r->ad == STATIC_AD)? "STATIC" : "CONNECTED",
		get_time(&r->last_update),
		dec_to_bin(r->flags)
	);
	return s;
}

void print_routing_table(){
	printf("-------- ROUTING TABLE --------\n");
	if (routes_ll == 0){
		printf("--------    END    --------\n");
		return;
	}
	Item * curr = (Item *) routes_ll->head;
	int i = 1;
	while(curr){
		printf("[%i] %s\n", i, get_route(R));
		curr = curr->next;
		i++;
	}
	printf("--------    END    --------\n");
}

void clear_routing_table(){
	// for each bla bla..
	free(routes_ll);
	routes_ll = 0;
}

// id 1 is the first
int routing_table_delete(Route * r, int id){
	if (routes_ll == 0){
		return 0;
	}
	if (id && r) {
		my_log("[R_TABLE]\t Cannot delete by route and id, choose one");
		return 0;
	}
	Item * curr = (Item *) routes_ll->head;
	int i;
	for(i = 1; curr; i++, curr = curr->next ){

		if ( id == 1 || id == 2 || (r && r->ad == DIRECTLY_CONNECTED_AD) ) {
			my_log("[R_TABLE]\t Cannot remove directly connected routes");
			return 0;
		}
		if (id && i == id) {
			my_log("[R_TABLE]\t Deleted by id");
			LL_delete(routes_ll, i-1);
			return 1;
		} else if (r && R == r) {
			my_log("[R_TABLE]\t Deleted by route");
			LL_delete(routes_ll, i-1);
			return 1;
		}
	}
	return 0;
}


Route * routing_table_search(u_int dst){
	if (routes_ll == 0){
		return 0;
	}
	sprintf(log_b, "[R_TABLE]\t routing_table_search: %s", ip_to_string(dst));
	my_log(log_b);
	Route * potencial = 0;
	Item * curr = (Item *) routes_ll->head;
	while(curr){
		// this entry is only as rip database,
		// do not preform real search with it
		if ( ! FLG_CHK(R->flags, RIP_FLAG_DB)) {
			if ( belongs_to_subnet(dst, R->network, R->mask) && R->ad > (potencial ? potencial->ad : -1) ){
				my_log("[R_TABLE]\t Found potencial route ");
				potencial = R;
			}
		}
		curr = curr->next;
	}
	if(potencial == 0) {
		my_log("[R_TABLE]\t NOT found");
	}
	return potencial;
}

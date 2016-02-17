
#define R ((Rule*)curr->data)

typedef struct Rule {
	int id;
	Port * port;
	int action; // ALLOW or DENY
	int direction; // IN or OUT
	int src_addr_type; // MAC or IP
	int dst_addr_type; // MAC or IP
	char * src_addr;
	char * dst_addr;
	int proto; // http, icmp, ...
}Rule;

LL * rules_ll = 0;

void mock_rule(){
    Rule * r = (Rule *) malloc(sizeof(Rule));
    r->src_addr = (char *) malloc(20);
    r->dst_addr = (char *) malloc(20);
    r->id = 0;
    r->port = p1;
    r->action = R_ALLOW;
    r->direction = R_IN;
    r->dst_addr = "any";
	r->src_addr = "C0:EE:FB:4A:C4:1D"; // oneplus2
    //r->src_addr = "84:38:35:45:eb:1a"; // duskis
    r->proto = R_ANY;
    r->src_addr_type = R_ANY;
    r->dst_addr_type = R_ANY;

    if (rules_ll == 0){
        rules_ll = LL_init();
    }
    LL_add_to_front(rules_ll, (void *)r);
}

void create_rule(char * port, char * action, char * direction, char * dst, char * src, char * proto){
    Rule * r = (Rule *) malloc(sizeof(Rule));
    r->src_addr = (char *) calloc(20, 1);
    r->dst_addr = (char *) calloc(20, 1);


    if (strcasecmp(port, "1") == 0) {
        r->port = p1;
    } else if (strcasecmp(port, "2") == 0){
        r->port = p2;
    } else {
        printf("Invalid input at port: '%s' \n", port);
        return;
    }

    if (strcasecmp(action, "allow") == 0) {
        r->action = R_ALLOW;
    } else if (strcasecmp(action, "deny") == 0){
        r->action = R_DENY;
    } else {
        printf("Invalid input at action %s\n", action);
        return;
    }

    if (strcasecmp(direction, "in") == 0) {
        r->direction = R_IN;
    } else if (strcasecmp(direction, "out") == 0){
        r->direction = R_OUT;
    } else {
        printf("Invalid input at direction\n");
        return;
    }

    if (strcasecmp(src, "any") == 0) {
        strcpy(r->src_addr, src);
        r->src_addr_type = R_ANY;
    } else if (is_valid_mac(src)) {
        strcpy(r->src_addr, src);
        r->src_addr_type = R_MAC;
    } else if (is_valid_ip(src)) {
        strcpy(r->src_addr, src);
        r->src_addr_type = R_IP;
    } else {
        printf("Invalid input at src\n");
        return;
    }

    if (strcasecmp(dst, "any") == 0) {
        strcpy(r->dst_addr, dst);
        r->dst_addr_type = R_ANY;
    } else if (is_valid_mac(dst)) {
        strcpy(r->dst_addr, dst);
        r->dst_addr_type = R_MAC;
    } else if (is_valid_ip(dst)) {
        strcpy(r->dst_addr, dst);
        r->dst_addr_type = R_IP;
    } else {
        printf("Invalid input at dst\n");
        return;
    }

    if (strcasecmp(proto, "any") == 0) {
        r->proto = R_ANY;
    } else {
        r->proto = atoi(proto);
    }

    if (rules_ll == 0){
        rules_ll = LL_init();
    }
    LL_add_to_front(rules_ll, (void *)r);

}

void print_rules(){
    if (rules_ll == 0){
        printf("\nNo rules added yet, you can do so by entering config mode, press 'c' and hit Enter\n");
        return;
    }

    Item * curr = (Item *) rules_ll->head;
    int i = 0;
    printf("\n%d Rule(s) are saved\n ID \t PORT \t ALLOW/DENY \t IN/OUT \t SRC_ADDR \t DST_ADDR \t PROTO\n", rules_ll->number_of_items);
    printf("----------------------------------------------------------------------------------------\n");
    while(curr){
        printf("%2i \t %i \t %s \t\t %s \t\t %s \t\t %s \t %i\n",
            i,
            R->port->id,
            ( R->action == R_ALLOW )? "allow" : "deny",
            ( R->direction == R_IN )? "in" : "out",
            R->src_addr,
            R->dst_addr,
            R->proto
        );
        curr = curr->next;
        i++;
    }

}
int apply_rules(Frame * f, Port * p, int direction){
    int forward = (DEFAULT_ACTION == R_ALLOW);
	my_log("\tApplying rules..");
    if (rules_ll == 0){
        return forward;
    }

    Item * curr = (Item *) rules_ll->head;
    for(; curr; curr = curr->next){

        // port fits
        if ( ! (R->port == p) ){
			sprintf(log_b, "\tWrong port %i", p->id);
			//my_log(log_b);
            continue;
        }

        // direction check
        if ( !(direction == R->direction) ) {
			sprintf(log_b, "\tWrong direction %i", direction);
			//my_log(log_b);
            continue;
        }

        // src address check
        if ( !(     strcasecmp(R->src_addr, "any") == 0
				||	strcasecmp(R->src_addr, get_src_mac(f)) == 0
				||	strcasecmp(R->src_addr, get_src_ip(f)) == 0
			) ) {
			sprintf(log_b, "\tWrong src");
			//my_log(log_b);
            continue;
    	}


		// dst address check
        if ( !(     strcasecmp(R->dst_addr, "any") == 0
				||	strcasecmp(R->dst_addr, get_dst_mac(f)) == 0
				||	strcasecmp(R->dst_addr, get_dst_ip(f)) == 0
			) ) {
			sprintf(log_b, "\tWrong dst");
			//my_log(log_b);
            continue;
    	}


        if ( !( R_ANY == R->proto || R->proto == f->l5 ) ) {
			//my_log("Wrong proto");
			continue;
        }

		my_log("\tRule applied");
		forward = (R->action == R_ALLOW);
		break;
    }

    return forward;
}

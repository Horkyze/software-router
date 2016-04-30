#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define CMD ((Command *)curr->data)
int set_interface_ip(char *);
int add_static_route(char *);
int list_commands(char * cmd);
int get_arp_cache(char *);
int delete_arp_cache(char * cmd);
int send_arp_request(char * cmd);
int rip_add_route(char * cmd);

LL * cmd_ll = 0;

typedef struct Command {
	char cmd[24];
	char description[1024];
	char example[1024];
	int (*execute)(char *);
}Command;

void create_cmd(char * cmd, char * des, char * exa, int (*exec)(char *)){
	if(cmd_ll == 0){
		cmd_ll = LL_init();
	}
	Command * c;
	c = (Command *) malloc(sizeof(Command));
	strcpy(c->cmd, cmd);
	strcpy(c->description, des);
	strcpy(c->example, exa);
	c->execute = exec;
	LL_add(cmd_ll, c);
}

int exec_cmd(char * cmd_name, char * arguments){
	if (!cmd_ll)
		return 0;
	Item * curr = (Item *) cmd_ll->head;
	while(curr){
		if (strcmp(CMD->cmd, cmd_name) == 0) {
			sprintf(log_b, "Executing: %s %s", cmd_name, arguments );
			my_log(log_b);
			int status = CMD->execute(str_trim(arguments));
			if (status) {
				my_log("Exec OK");
			} else {
				my_log("Exec FAILED");
			}
			return status;
		} else {
			curr = curr->next;
		}
	}
	printf("cmd not found, type '?' for help\n");
	return 0;
}

char * parse_command(char * cmd){
	char args[100];
	char * name;
	name = split_cmd(cmd, args);
	if (exec_cmd(name, args) == 0){
		strcpy(response, "invalid command madafaka");
	}
	return 0;
}

void init_commands(){
	create_cmd("get_arp_cache",
				"get_arp_cache ",
				"get_arp_cache",
				&get_arp_cache
			);
	create_cmd("delete_arp_cache",
				"delete_arp_cache ",
				"delete_arp_cache",
				&delete_arp_cache
			);
	create_cmd("send_arp_request",
				"send_arp_request <ip>",
				"send_arp_request 10.0.0.2",
				&send_arp_request
			);
	create_cmd("ip_address",
				"ip_address <interface> <ip> <prefix>",
				"ip_address p1 10.0.0.1 24",
				&set_interface_ip
			);
	create_cmd("network",
				"network <network> <prefix> <outgoing_interface>",
				"network 192.168.0.0 16 p2",
				&add_static_route
			);
	create_cmd("rip",
				"rip <network>",
				"rip 10.0.1.0",
				&rip_add_route
			);
	create_cmd("?",
				"? - list all possible commands",
				"?",
				&list_commands
			);
}

int list_commands(char * args){
	if (!cmd_ll)
		return 0;
	Item * curr = (Item *) cmd_ll->head;
	while(curr){
		printf("%s\n", CMD->cmd);
		printf("  %s\n", CMD->description);
		printf("  %s\n\n", CMD->example);
		curr = curr->next;
	}
	return 1;
}

int get_arp_cache(char * cmd){

	Item * curr = (Item *) arp_cache_ll->head;
	char part[150];
	strclr(part);
	if ( ! curr ){
		strcat(response, "arp empty");
	}
	while(curr){
		printf("mac: %s\n", get_hex(A->mac, 6, ':'));
		sprintf(part, "%s \tis at %s\n", ip_to_string(A->ip), get_hex(A->mac, 6, ':'));
		strcat(response, part);
		curr = curr->next;
	}
	return 1;
}

int delete_arp_cache(char * args){
	arp_cache_ll = LL_init();
	strcpy(response, "cache deleted");
	return 1;
}

int send_arp_request(char * args){
	u_char * reply = arp_get(string_to_ip(args), p1);
	printf("REPLY: %s\n", reply);
	return 1;
}


// ip_address <interface> <ip> <prefix>
int set_interface_ip(char * args){
	char interface_s[2];
	Port * interface;
	char ip_s[15];
	int prefix;

	sscanf(args, "%s %s %i", interface_s, ip_s, &prefix );

	if (strcmp(interface_s, "p1") == 0) {
		interface = p1;
	} else {
		interface = p2;
	}

	interface->ip = string_to_ip(ip_s);
	interface->mask = prefix;

	return 1;
}

// network <network> <prefix> <outgoing_interface>
int add_static_route(char * args){
	char * ip_s;
	char * prefix;
	char * interface_s;
	Port * interface;

	ip_s = strsep(&args, " ");
	prefix = strsep(&args, " ");
	interface_s = strsep(&args, " ");
	//printf("ARGS: '%s'\n", args);
	//sscanf(args, "%s %s %s", ip_s, prefix, interface_s);
	//printf("parsed: '%s' '%s' '%s'", ip_s, prefix, interface_s);

	if (strcmp(interface_s, "p1") == 0) {
		interface = p1;
	} else {
		interface = p2;
	}
	add_route(string_to_ip(ip_s), atoi(prefix), interface, STATIC_AD, 0);
	return 1;
}

// rip <network>
int rip_add_route(char * args){
	u_int net = inet_addr( str_trim(args) );
	Route * r = routing_table_search(net);
	if (! r){
		my_log("[RIP-CONFIG]\t Cannot add network that is not in routing table");
		strcpy(response, "failed");
		return 0;
	}
	int mask = r->mask;
	Port * out_port = r->outgoing_interface;
	add_route(net, mask, out_port, RIP_AD, RIP_FLAG_DB);
	return 1;
}



/* This is our thread function.  It is like main(), but for a thread*/
void * config(void * arg){

	char c;
	char * line;
	char * token;
	char * cmd_name;
	char * arguments = (char *)malloc(100);
	init_commands();
	while (1) {
		scanf("%c", &c);
		if (c == 'c') {
			// now user is in config mode
			pause_rendering = 1;

			while(1) {
				memset(arguments, 0, 100);
				line = get_line();
				if (strcmp("q", line) == 0) {
					pause_rendering = 0;
					break;
				}
				cmd_name = split_cmd(line, arguments);
				if (cmd_name){
					if (exec_cmd(cmd_name, arguments)){
						printf("OK\n");
					} else {
						printf("ERROR\n");
					}
				}
				printf("config> ");
			}
		} else if (c == 'm'){ // mock data

            //my_log(arp_get(string_to_ip("10.0.0.2"), p1));

			//add_route(string_to_ip("10.1.0.0"), 16, p1, STATIC_AD, 0);
		} else if (c == 'r'){ // mock data

            //read_mapped_mem(MEM_IN_FILE);
		}

	}

}

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#define CMD ((Command *)curr->data)
int set_interface_ip(char *);
int add_static_route(char *);
int list_commands(char * cmd);

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

void exec_cmd(char * cmd_name, char * arguments){
	if (!cmd_ll)
		return;
	Item * curr = (Item *) cmd_ll->head;
	while(curr){
		if (strcmp(CMD->cmd, cmd_name) == 0) {
			CMD->execute(arguments);
			return;
		} else {
			curr = curr->next;
		}
	}
	printf("cmd not found, type '?' for help\n");
}

void init_commands(){
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
	return 0;
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
	
	return 0;
}

// network <network> <prefix> <outgoing_interface>
int add_static_route(char * args){

	char ip_s[15];
	int prefix;
	char interface_s[2];
	Port * interface;

	sscanf(args, "%s %i %s", ip_s, &prefix, interface_s);

	if (strcmp(interface_s, "p1") == 0) {
		interface = p1;
	} else {
		interface = p2;
	}

	add_route(string_to_ip(ip_s), prefix, interface, STATIC_AD, 0);

	return 0;
}



/* This is our thread function.  It is like main(), but for a thread*/
void * config(void * arg){

	char c;
	char * line;
	char * token;
	char * cmd_name;
	char * arguments = malloc(100);
	init_commands();
	while (1) {
		scanf("%c", &c);
		if (c == 'c') {
			// now user is in config mode
			pause_rendering = 1;

			while(1){
				memset(arguments, 0, 100);
				line = get_line();
				if (strcmp("q", line) == 0) {
					pause_rendering = 0;
					break;
				}
				cmd_name = split_cmd(line, arguments);
				if (cmd_name)
					exec_cmd(cmd_name, arguments);
				printf("config> ");
			}
		} else if (c == 'm'){ // mock data
			add_route(string_to_ip("10.1.0.0"), 16, p1, STATIC_AD, 0);
		}

	}

}

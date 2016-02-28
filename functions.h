#ifndef FUNSTIONS_H
#define FUNSTIONS_H

void my_log(const char * msg){

	time_t t;
	time(&t);
	char time[100];

	strcpy(time, ctime(&t));
	time[strlen(time) - 4] = '\0';
	FILE * fd;

	fd = fopen(LOG_FILE, "a");
	if (fd == 0) {
		printf("fd is zero \n");
	}
    fprintf(fd, "%s\b || %s \n", time, msg );
	fclose(fd);
}

char * get_time(time_t * tt){
    char * t = (char *) calloc(1, 20);
    t = ctime(tt);
    memset(t, 0, 11);
    memset(t+20, 0, 1);
    return (char *)(t +11);
}

int is_print(u_char c){
	return (c > 31 && c <= 127);
}


/* return 1 if string contain only digits, else return 0 */
int valid_digit(char *ip_str)
{
    while (*ip_str) {
        if (*ip_str >= '0' && *ip_str <= '9')
            ++ip_str;
        else
            return 0;
    }
    return 1;
}

/* return 1 if IP string is valid, else return 0 */
int is_valid_ip(char *ip_str)
{
    int i, num, dots = 0;
    char *ptr;

    if (ip_str == NULL)
        return 0;

    ptr = strtok(ip_str, ".");

    if (ptr == NULL)
        return 0;

    while (ptr) {

        /* after parsing string, it must contain only digits */
        if (!valid_digit(ptr))
            return 0;

        num = atoi(ptr);

        /* check for valid IP */
        if (num >= 0 && num <= 255) {
            /* parse remaining string */
            ptr = strtok(NULL, ".");
            if (ptr != NULL)
                ++dots;
        } else
            return 0;
    }

    /* valid IP string must contain 3 dots */
    if (dots != 3)
        return 0;
    return 1;
}

int is_valid_mac(char * mac) {
    int i = 0;
    int s = 0;

    while (*mac) {
       if (isxdigit(*mac)) {
          i++;
       }
       else if (*mac == ':' || *mac == '-') {

          if (i == 0 || i / 2 - 1 != s)
            break;
          ++s;
       }
       else {
           s = -1;
       }
       ++mac;
    }

    return (i == 12 && (s == 5 || s == 0));
}

char * get_hex(u_char * data, int size, char delimiter){
	int i;
	char * buff = (char * ) calloc(3, size);
	for (i = 0; i < size; ++i){
		if(i == size - 1)
			sprintf(&buff[3*i], "%02X", data[i]);
		else
			sprintf(&buff[3*i], "%02X%c", data[i], delimiter);
	}
	return buff;
}

void print_hex(u_char * data, int len){
	int i;
	for (i = 0; i < len; ++i)
	{
		printf("%02x ", data[i]);
	}
	printf("\n");
}

char * ip_to_string(u_int ip){
	char * s;
	s = (char *) malloc(20);
    u_char bytes[4];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;
    sprintf(s, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
    return s;
}

u_int string_to_ip(char * s){
	char * str, * saveptr;
	str = (char *) malloc(strlen(s));
	strcpy(str, s);
	int shift = 0;
	u_int ip = 0;
	short t = 0;
	char * token = 0;

	for (;; str = NULL) {
		token = strtok_r(str, ".", &saveptr);
		if (token == NULL)
			break;
		t = (unsigned short) atoi(token);
	 	ip |= t << shift;
	 	shift += 8;
	 }

	return ip;
}

int mask_to_prefix(){

	return 0;
}

u_int prefix_to_mask(int prefix){
	return 0xffffffff << (32-prefix);
	// char * s;
	// s = (char *) malloc(15);
	// u_char * bytes;
	// u_int mask = 0xffffffff;
	// mask = mask << (32-prefix);
	// bytes = (u_char *) &mask;
	// sprintf(s, "%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
	// return s;
}

int belongs_to_subnet(u_int addr, u_int subnet, int prefix){
	return (addr << (32-prefix)) >> (32-prefix) == subnet;
}


void dump(const unsigned char * p, int len){
	int i, k = 0;
	u_char buff[17];
	memset(buff, 0, 17);
	for (i = 1; i < len + 1; ++i)
	{
		printf("%02x ", p[i-1]);

		buff[(i-1)%16] = (is_print(p[i-1]))? p[i-1] : '.';

		if (i % 8 == 0 && i) {
			printf(" ");
		}


		if (i % 16 == 0 && i){

			printf(" | %s \n", buff);
			memset(buff, 0, 17);
			k = 0;
		}


	}
	printf("\n\n");
}

char * split_cmd(char * s, char * arguments){
	char * name = 0;
	char * pos;
	pos = strchr(s, ' ');
	if(pos){
		name = (char *) malloc(pos - s);
		strncpy(name, s, pos - s);
		strcpy(arguments, pos);
	}
	return name;
}

char * get_line() {
    char * line = malloc(100), * linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if(line == NULL)
        return NULL;

    for(;;) {
        c = fgetc(stdin);
        if(c == EOF)
            break;

        if(--len == 0) {
            len = lenmax;
            char * linen = realloc(linep, lenmax *= 2);

            if(linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if((*line++ = c) == '\n')
            break;
    }
    *(line - 1) = '\0'; // dont save new line character
    return linep;
}

void memswap(void * a, void * b, int size){
	void * temp = malloc(size);
	// dst, src
	memcpy(temp, a, size);
	memcpy(a, b, size);
	memcpy(b, temp, size);
	free(temp);
}


/*===================================
=            Linked List            =
===================================*/

typedef struct LL {
	void * head;
	u_int number_of_items;
    int auto_increment;
}LL;

typedef struct Item {
	unsigned int index;
	void * data;
	struct Item * next;
}Item;

LL * LL_init(){
	LL * ll = malloc(sizeof(LL));
	ll->head = 0;
	ll->number_of_items = 0;
    ll->auto_increment = 0;
	return ll;
}

Item * LL_add(LL * ll, void * data){
	Item * item = malloc(sizeof(Item));
	item->data = data;
	item->next = 0;
    ll->number_of_items++;
	ll->auto_increment++;

	if (ll->head == 0){
		ll->head = item;
		item->index = 1;
		return item;
	}

	Item * curr = ll->head;
	int i = 0;
	while(curr->next != 0) {
	    curr = curr->next;
	    i++;
	}
	curr->next = item;

	item->index = ll->number_of_items;
	return item;
}

Item * LL_add_to_front(LL * ll, void * data){
    Item * item = malloc(sizeof(Item));
    item->data = data;
    item->next = ll->head;

    ll->head = item;
    ll->number_of_items++;
    ll->auto_increment++;
    return item;
}

// delete nth item from list, 0 is the first element
void LL_delete(LL * ll, int n)
{
    if(n > ll->number_of_items || n < 0)
        return;

    Item * curr, *prev;
    int i;
    curr = ll->head;
    prev = 0;

    for (i = 0; curr; ++i){
        if (i == n){
            // delete this
            if (prev){
                prev->next = curr->next;
            } else {
                ll->head = curr->next;
            }
            free(curr);
            ll->number_of_items--;
            return;
        }

        prev = curr;
        curr = curr->next;
    }

}

void LL_print(LL * ll){
	Item * curr = ll->head;
	while(curr) {
	    printf("Item index %i: data_p: %p next_p: %p \n", curr->index, curr->data, curr->next);
	    curr = curr->next;
	}
}

/*-----  End of Linked List  ------*/


#endif

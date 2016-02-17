
/*==================================
=        HASH TABLE FOR IP         =
==================================*/

typedef struct Item_h{
	void * data; // pointer to data

}Item_h;

typedef struct Hash_table{
	Item_h * table;	// array of Item_h 
	int data_size; // size of data, item_h->data points to
	u_int * indexes;  // occupied indexed in table
	u_int current_index_pointer;
	u_int n;
}Hash_table;


Hash_table * init_hash_table(int size){

	Hash_table * h = 0;
	h = (Hash_table*) malloc(sizeof(Hash_table)); 

	h->n = size;
	h->table  = 0;
	h->current_index_pointer = 0;

	h->table  	= (Item_h *) calloc(sizeof(Item_h), size); 
	h->indexes  = (int *)    malloc(sizeof(int) * size); 

	memset(h->table , 0, sizeof(Item_h) * size);

	return h;
}

u_int hash(void * data, int size){
	u_int a = 0;
	memcpy(&a, data, (size > sizeof(a)? sizeof(a) : size) );
	a ^= (a << 13);
	a ^= (a >> 17);
	a ^= (a << 5);
	return (a == 0)? 1 : a; // never return 0
}

// insert into hash table ht value x

int insert_h(Hash_table * ht, void * data, int size)
{
	Item_h * item = (Item_h * ) malloc(sizeof(Item_h));
	memcpy(item->data, data, size);

	// collision ??

		//duplicate entry

		//new entry



	u_long h = hash(data, size) % ht->n;
	if ( (ht->table[h])->data == 0){
		(ht->table[h])->data = data;
	} 

	(ht->table[h]).bytes_send += bytes;
	(ht->table[h]).ip = x;
	ht->indexes[ ht->current_index_pointer++ ] = h;


	return 1;
}


// search in hash table ht for value x
// return 1 if found
int search_h(Hash_table * ht, u_int x)
{
	u_long h = hash(x) % ht->n;

	while((ht->table[h]).ip != x){
		
		if ((ht->table[h]).ip == 0){
			return 0;
		}

		h = (h < ht->n)? h + 1 : 0;
	}

	return ht->table[h].bytes_send;

}



/*-----  End of HASH TABLE  ------*/










/*==================================
=        HASH TABLE                =
==================================*/

typedef struct ITEM_HT{
	LL * ll;
	u_int conversation_id;
}ITEM_HT;

typedef struct HT{
	ITEM_HT * table;	// array of ITEM_HT 
	u_int * indexes;  // occupied indexed in table
	u_int current_index_pointer;
	u_int n;
}HT;



HT * init_ht(int size){

	HT * h = 0;
	h = (HT*) malloc(sizeof(HT)); 

	h->n = size;
	h->table  = 0;
	h->current_index_pointer = 0;

	h->table  	= (ITEM_HT *) malloc(sizeof(ITEM_HT) * size); 
	h->indexes  = (int *)    malloc(sizeof(int) * size); 

	memset(h->table , 0, sizeof(ITEM_HT) * size);
	while(size--){
		(h->table[size]).ll = LL_init();
		(h->table[size]).conversation_id = 0;
	}
	return h;
}



// find conversation id for frame f
// search for linked list in hash table
// add frame to linked list
LL * insert_ht(HT * ht, u_int conversation_id)
{

	u_long h = hash(conversation_id) % ht->n;
	(ht->table[h]).conversation_id = conversation_id;

	ht->indexes[ ht->current_index_pointer++ ] = h;


	return (ht->table[h]).ll;
}


// search in hash table ht for value x
// return 1 if found
// LL * search_ht(HT * ht, u_int x)
// {
// 	u_long h = hash(x) % ht->n;

// 	while((ht->table[h]).ip != x){
		
// 		if ((ht->table[h]).ip == 0){
// 			return 0;
// 		}

// 		h = (h < ht->n)? h + 1 : 0;
// 	}

// 	if (ht->table[h].conversation_id == 0){
// 		return 0;
// 	}

// 	return ht->table[h].ll;

// }



/*-----  End of HASH TABLE  ------*/

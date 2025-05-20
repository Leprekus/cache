#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
* specs: 
* Memory Locations hold 1 byte
* RAM: 64kb (2^19 bits)
* Cache: 1kb cache (2^13 bits)
*
* word size: 2 bytes (16 bits)
* word addressable
*
*/
#define byte uint8_t
#define word uint16_t 


typedef struct {
	word address; //cpu -> ram
	word data; // cpu <-> ram
	word control; //cpu
} BUSES;


#define MAX_ADDRESS 0x10000 
#define RAM_LINES 1024 
#define RAM_LINE_LENGTH 64
byte RAM[MAX_ADDRESS];

/*
 * The cache has m lines and n blocks such that m * n = cache size
 * The number of lines and number of blocks (contained in a line)
 * should not exceed the cache size
 *
 * Cache is 2-way set-associative
 * we have 16-bit addreses, the address is split as follows:
 * 
 * ---------------------------------------------------------------------
 * | 			16-bit adress           			|
 * |--------------------------------------------------------------------|
 * | 6 tag-bits | 4 set-index bits (0..15) | 6 byte-offset bits (0..63) | 
 * ----------------------------------------------------------------------
 * ------------------------------------------
 * | 		8-bit store		    |
 * |----------------------------------------|
 * | 6-tag bits | 1 valid bit | 1 dirty bit |
 * ------------------------------------------
 * */
#define CACHE_SIZE 4096 // 4096 bytes 
#define CACHE_NUM_LINES 64 //64 lines, 16 sets each containing 4 lines, each line is 128 bytes long
#define CACHE_NUM_BLOCKS 64 //64 bytes


const int CACHE_NUM_SETS = 16; // 64 lines / 4-way sets = 16 sets
typedef struct {
	byte store[CACHE_NUM_LINES]; // we store 1 extra byte per line to store tag, and line's status
	byte memory[CACHE_NUM_LINES][CACHE_NUM_BLOCKS];
} CACHE;

void fetch_byte(word address){}

/*
*word = 0b kkkk kkii iikk kkkk, 
* the i bits indicate the set number for that block 
* such that 0 <= set number <= 15
* a set has 4 lines meaning the ret value..ret value + 3
* are all valid lines for a given set
*/
int get_set_index(word address){ 
	//0b0000001111000000
	word index = (address & 0x03C0) >> 6;
	//since we have 4 lines per set we offset  the set's start
	return index; 
}

/* we get the lower bound of the current address by zeroing it*/
int get_address_start(word address){
	//binary = 0b1111 1111 1100 0000
	int lower_bound = address & 0xFFC0;
	return lower_bound;
}

/* we get the upper bound of the current address by setting 6 LSBs*/
int get_address_end(word address){
	int upper_bound = address | 0x003F;
	return upper_bound;
}

byte get_tag(word address){
	//in a 16-bit address we want to retrieve the 6 MSBs used as the tag
	word tag = address >> 10;
	return tag;
}

int get_offset(word address){
	// 0b0000 0000 0011 1111
	int offset = (address & 0x003F);
	return offset;
}
/*
 * once we find our set index (which can be thought of as a bucket containing 4 lines each)
 * More specifically, it contains <num lines per set>*ith and <num lines per set>*ith+<num lines per set> - 1 lines, 
 * where i is the set index
 * we perform a tag comparison in these two lines to determine whether
 * it is a cache hit or miss
 * */

//align tag bits with store's tag and check if data is valid (second LSB is set)
#define COMPARE_TAG(x, y) ((x) == (((y) << 2) | 0x02))
int is_cache_hit(word address, CACHE *c){
	int set_index = get_set_index(address);
	byte tag = get_tag(address);
	return (
		COMPARE_TAG(c->store[set_index + 0], tag) ||
		COMPARE_TAG(c->store[set_index + 1], tag) ||
		COMPARE_TAG(c->store[set_index + 2], tag) ||
		COMPARE_TAG(c->store[set_index + 3], tag) 
		
	);
}


/*
 * RAM memory map: https://www.researchgate.net/figure/RAM-Memory-Map-213-Ports-The-original-8051-had-four-eight-pin-general-purpose_fig2_291196461
 *
 * RAM will be divided into 'lines' that are the length of our cache-lines
 * so whenever an address is retrieved, the whole line where
 * that address..address + 128  will be moved into cache for cache-locality.
 * */
#define SAVE_TO_STORE(x, y) ((x) = ((y) << 2 | 0x02))
void load_block_to_cache(word address, CACHE *c){
	int set_index = get_set_index(address);
	int lower = get_address_start(address);
	int upper = get_address_end(address);
	byte tag = get_tag(address);
	SAVE_TO_STORE(c->store[set_index], tag);
	for(int i = lower; i < upper; i++){
		c->memory[set_index][upper - i] = RAM[i];
	}

};


void system_init(CACHE *c){
	for(int i = 0; i < MAX_ADDRESS; i++){
		RAM[i] = 0x00;
	}
	for(int i = 0; i < CACHE_NUM_LINES; i++){
		c->store[i] = 0x00;
		for(int j = 0; j < CACHE_NUM_BLOCKS; j++){
			c->memory[i][j] = 0x00;
		}
	}
}
/*testing that the address distribution across sets is correct*/
void test_get_set_index(){
	int set[16] = {0};
	//map each address to one of the sets
	for(int address = 0; address < MAX_ADDRESS; address++){
		set[ get_set_index(address) ]++;

	}
	int total = 0;
	for(int i = 0; i < 16; i++){
		total += set[i];
		assert(set[i] == 4096);
        	printf("Set %d: %d addresses\n", i, set[i]);
    	}
	//assert(total == 65536);
	printf("total %d / 65536\n", total);
	printf("test_get_set_index is successful\n");
}
void test_address_start_end(){
	for(int address = 0; address < MAX_ADDRESS; address++){
		int upper = get_address_end(address);
		int lower = get_address_start(address);
		assert(upper - lower == 63);
	}
	printf("test_address_start_end is successful\n");
}
/*test 64 blocks are mapped to each set and loaded correctly
 * if the address is not in the cache it should be loaded to it
 * and all local addresses should be fetched from the cache
 * */
void test_locality(){
	CACHE c;
	system_init(&c);
	for(int address = 0; address < MAX_ADDRESS; address++){
		int lower = get_address_start(address) + 1;
		int upper = get_address_end(address);
		//address should be in cache
		if(lower <= address && address <= upper) 
			assert(is_cache_hit(address, &c) == 1);	
		//cache miss and address is loaded into cache
		else { 
			assert(is_cache_hit(address, &c) == 0);
			load_block_to_cache(address, &c);
		}
		
	}
	printf("test_locality is successful\n");
	
}
void test_is_cache_hit(){
	CACHE c;

	assert(is_cache_hit(0xCAFE, &c) == 0);
	
	
	//generate random index from 0..3 to make sure we are looking in the 4 lines of the set
	srand(time(NULL)); 
	int noise = rand() % 4; 
	int set_index = get_set_index(0xBABE) + noise;

	//load the cache with some data
	int lower = get_address_start(0XBABE);
	int upper = get_address_end(0xBABE);
	for(int i = lower; i < upper; i++) c.memory[set_index][upper - i] = 'a';	
	//store the tag in the cache store
	byte tag = get_tag(0XBABE);
	// we shift 2 because the 2 LSBs are reserved for policy and data validity
	SAVE_TO_STORE(c.store[set_index], tag);
	assert(is_cache_hit(0xBABE, &c) == 1);
	printf("test_is_cache_hit is successful\n");
}

void load_data(const byte data[], size_t len){
	for(int i = 0; i < len; i++){
		RAM[i] = data[i]; 
	}
}
void dump_memory(){
	
}

void unit_tests() {
	test_get_set_index();
	test_address_start_end();
	test_locality();
	test_is_cache_hit();
}
int main () { 
	const byte string[13] = { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\n', '\0' };
	CACHE c;
	system_init(&c);
	load_data(string, 13);
	dump_memory();
	unit_tests();	
	return 0; 
}


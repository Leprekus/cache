#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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



#define MAX_ADDRESS 0x10000 //65kb
#define RAM_LINES 1024 //1024 * 64 = 65,536
#define RAM_LINE_LENGTH 64
/**
 * Even though in a real system the memory is contigouous,
 * this abstraction makes determining the starting and ending
 * addresses for cache locality much easier. As zeroing the block offest (RAM_LENGTH)
 * gives the starting address, and one ing it gives the ending address
 */
word RAM[RAM_LINES][RAM_LINE_LENGTH];

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
 * | 7 tag-bits | 4 set-index bits (0..15) | 5 byte-offset bits (0..63) | 
 * ----------------------------------------------------------------------
 * */
#define CACHE_SIZE 4096 // 4096 bytes 
#define CACHE_NUM_LINES 64 //64 lines, 16 sets each containing 4 lines, each line is 128 bytes long
#define CACHE_NUM_BLOCKS 64 //64 bytes


const int CACHE_NUM_SETS = 16; // 64 lines / 4-way sets = 16 sets
typedef struct {
	byte store[CACHE_NUM_LINES]; // we store 1 extra byte per line to store tag, and line's status
	byte memory[CACHE_NUM_BLOCKS][CACHE_NUM_BLOCKS];
} CACHE;

/*
 * RAM memory map: https://www.researchgate.net/figure/RAM-Memory-Map-213-Ports-The-original-8051-had-four-eight-pin-general-purpose_fig2_291196461
 *
 * RAM will be divided into 'lines' that are the length of our cache-lines
 * so whenever an address is retrieved, the whole line where
 * that address..address + 128  will be moved into cache for cache-locality.
 * */
void load_block_to_cache(word address){};

void fetch_byte(word address){}

/*
*word = 0b kkkk kkii iiii kkkk, 
*the i bits indicate the S_ith collection to which the range of addresses belongs to
*since each collection consists of 64 addresses we have 1024 collections
*these 10 significant bits (i) needed to represent the number 1024 
*indicate the "collection's number"
*we map this collection to a set by doing i % num_of_buckets
*/
int get_set_index(word address){ 
	//0b0000001111000000
	word index = (address & 0x3C0) >> 6;
	//since we have 4 lines per set we offset  the set's start
	return index * 4; 
}

/* we get the lower bound of the current address by zeroing it*/
int get_address_start(word address){
	//binary = 0b1111 1111 1100 0000
	int lower_bound = address & 0xFFC0;
	return lower_bound;
}

/* we get the upper bound of the current address by one-ing it*/
int get_address_end(word address){
	int upper_bound = address & 0xFFFF;
	return upper_bound;
}

int get_block_index(word address){
	//0b1111110000000000
	word tag = (address & 0xFC00) >> 10;
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
int is_cache_hit(word address, CACHE *c){
	int set_index = get_set_index(address);
	int tag = get_block_index(address);
	int offset = get_offset(address);
	return (
		(c->memory[set_index + 0][0] & tag) ||
		(c->memory[set_index + 1][0] & tag) ||
		(c->memory[set_index + 2][0] & tag) ||
		(c->memory[set_index + 3][0] & tag) 
		
	);
}


/*testing that the address distribution across sets is correct*/
void test_get_set_index(){
	int count[16] = {0};
	for(int address = 0; address < MAX_ADDRESS; address++){
		count[ get_set_index(address) ]++;

	}
	int total = 0;
	for(int i = 0; i < 16; i++){
		total += count[i];
		assert(count[i] == 4096);
        	printf("Set %d: %d addresses\n", i, count[i]);
    	}
	assert(total == 65536);
	printf("total %d / 65536\n", total);
	printf("test_get_set_index is successful\n");
}
/*test 64 blocks are mapped to each set and loaded correctly
 * if the address is not in the cache it should be loaded to it
 * and all local addresses should be fetched from the cache
 * */
void test_blocks(){
	for(int address = 0; address < MAX_ADDRESS; address++){
	
	}
	
}
void test_is_cache_hit(){
	CACHE c;
	assert(is_cache_hit(0xCAFE, &c) == 0);

	assert(is_cache_hit(0xBABE, &c) == 1);
	printf("test_is_cache_hit is successful\n");
}
int main () { 
	const byte string[13] = { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\n', '\0' };
	test_get_set_index();

	return 0; 
}


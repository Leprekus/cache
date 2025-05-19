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
 * ------------------------------------------------------
 * | 16-bit adress           				|
 * |----------------------------------------------------|
 * | 6 tag-bits | 6 byte-offset bits | 4 set-index bits | 
 * ------------------------------------------------------
 * */
#define CACHE_SIZE 4096 // 4096 bytes 
#define CACHE_NUM_LINES 64 //64 lines, 16 sets each containing 4 lines, each line is 128 bytes long
#define CACHE_NUM_BLOCKS 64 //64 bytes


const int CACHE_NUM_SETS = 16;
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
void load_ram_line_to_cache(){};

/*
 * we have 64 lines / 4 way sets = 16 total sets
 * so we want to bound our address to one of those 16 sets
 * such that 0 <= address < 16
 * */
int get_set_index(word address){ return address % CACHE_NUM_SETS; }

/*
 * once we find our set index (which can be thought of as a bucket containing 4 lines each)
 * More specifically, it contains <num lines per set>*ith and <num lines per set>*ith+<num lines per set> - 1 lines, 
 * where i is the set index
 * we perform a tag comparison in these two lines to determine whether
 * it is a cache hit or miss
 * */
int is_cache_hit(word address, CACHE *c){
	int set_index = get_set_index(address);
	int i = 4 * set_index; //calculated as: <number of lines per set> * i, where i is the ith set
	/*
	 * grab the upper 4 bits (MSBs are unique) of the address and move it to the 
	 * lower 8 bits to prevent overflow when storing in byte
	 * */
	byte tag = (address & 0xF000) >> 12; 
	return (
		(c->store[i] & tag) ||
		(c->store[i + 1] & tag) ||
		(c->store[i + 2] & tag) ||
		(c->store[i + 3] & tag) 
	);
}


void test_get_set_index(){
	for(int i = 0; i < MAX_ADDRESS; i++)
		assert(get_set_index(i) <= 16);
	printf("test_get_set_index is successful\n");
}
void test_is_cache_hit(){
	CACHE c;
	assert(is_cache_hit(0xCAFE, &c) == 0);

	c[0xBABE] = 
	assert(is_cache_hit(0xBABE, &c) == 1);
	printf("test_is_cache_hit is successful\n");
}
int main () { 
	const byte string[13] = { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\n', '\0' };
	test_get_set_index();

	return 0; 
}


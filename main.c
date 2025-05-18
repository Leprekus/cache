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
typedef struct {
	word address[ MAX_ADDRESS ];
	word size; //keep track of addresses occupied
} RAM;

/*
 * The cache has m lines and n blocks such that m * n = cache size
 * The number of lines and number of blocks (contained in a line)
 * should not exceed the cache size
 * */
#define CACHE_SIZE 0x2000 //1kb -> 8192 bits
#define CACHE_NUM_LINES 0x80 //128 bytes
#define CACHE_NUM_BLOCKS 0x40 //64 bytes

word CACHE[CACHE_NUM_LINES][CACHE_NUM_BLOCKS];

/*
 * RAM memory map: https://www.researchgate.net/figure/RAM-Memory-Map-213-Ports-The-original-8051-had-four-eight-pin-general-purpose_fig2_291196461
 *
 * RAM will be divided into 'lines' that are the length of our cache-lines
 * so whenever an address is retrieved, the whole line where
 * that address is located will be moved into cache for cache-locality.
 * */
void load_ram_line_to_cache(){};

void mem_init(RAM *r) {
	memset(r->address, 0, MAX_ADDRESS);
	r->size = 0;
}


//load 11 chars: 'hello world\n' from address[0x00:0x0B]
void load_to_main_memory(RAM *r, const byte data[], word size) {
	for(word i = 0; i < size; i++){
		r->address[i] = data[i];
		r->size += 1;
	}
}

void dump_main_memory(RAM *r) {
	for(word i = 0; i < r->size; i++)
		printf("%c", r->address[i]);

}
int main () { 
	RAM r;
	CACHE c;
	const byte string[13] = { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\n', '\0' };
	load_to_main_memory(&r, string, 13); 
	dump_main_memory(&r);	
	return 0; 
}


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
word RAM[MAX_ADDRESS];

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
 * | 4 tag-bits | 6 byte-offset bits | 6 set-index bits | 
 * ------------------------------------------------------
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




int main () { 
	const byte string[13] = { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\n', '\0' };

	return 0; 
}


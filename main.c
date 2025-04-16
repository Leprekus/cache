#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/*
* specs: 
* 64kb ram
* 8mb cache
*
* byte addressable ram
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
typedef struct {
	word address[ MAX_ADDRESS ];
	word size;
} RAM;

const int MAX_NUM_LINES = 250000; // 8mb / 32b = 250_000
const int MAX_LINE_ADDR = 0x3D090; // 250_000
const int WIDTH_LINE = 32; //line width in bytes
typedef struct {
	word line[ 32 ];
} CACHE;

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


#include <inttypes.h>
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
	word addr[ MAX_ADDRESS ];	
} RAM;

const int MAX_NUM_LINES = 250000; // 8mb / 32b = 250_000
const int MAX_LINE_ADDR = 0x3D090; // 250_000
const int WIDTH_LINE = 32; //line width in bytes
typedef struct {
	word line[ 32 ];
} CACHE;

void mem_init(RAM *r) {
	memset(r->addr, 0, MAX_ADDRESS);
}
int main () { 
	RAM r;
	CACHE c;
	return 0; 
}


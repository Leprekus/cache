# Cache Simulator

This project implements a simple memory/cache system simulation in C with unit testing. It models RAM and a 2-way set-associative cache with basic memory access functions and testing utilities.

## Specs

- **RAM**: 64KB (word-addressable, 16-bit)
- **Cache**: 4KB total size  
  - 2-way set-associative  
  - 16 sets × 4 lines/set  
  - 64B per cache block
- **Address breakdown**:
  - 6 tag bits
  - 4 set index bits
  - 6 byte offset bits

## Structure

- `RAM[MAX_ADDRESS]`: Simulated main memory
- `CACHE`: Contains data storage and line status
- `BUSES`: Represents CPU-RAM communication (not fully used yet)

## Key Features

- Cache hit/miss detection
- Block loading from RAM to cache
- Bit masking and address manipulation
- Set index, tag, and offset extraction
- Unit tests for correctness

## Functions

- `get_set_index()`: Computes cache set index from address
- `get_address_start()/end()`: Determines block boundaries
- `get_tag()`: Extracts tag bits from address
- `is_cache_hit()`: Determines if address is in cache
- `load_block_to_cache()`: Loads block from RAM to cache
- `unit_tests()`: Runs all tests to verify logic

## How to Run

```bash
gcc -o cache_sim cache_sim.c
./cache_sim
```

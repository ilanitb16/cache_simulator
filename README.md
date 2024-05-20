# Cache Simulation Assignment

## Introduction

This repository contains code for simulating a cache memory system. The cache is implemented with support for read and write operations, as well as LFU (Least Frequently Used) replacement policy.
Imnplemented for my Computer Architecture course in Bar Ilan University.
![image](https://github.com/ilanitb16/cache_simulator/assets/97344492/7750200d-6348-429e-9363-66c12d967197)

## Features

- **Read Operation**: Simulates a read operation from the cache. If the requested data is not present in the cache (cache miss), it fetches the data from main memory and updates the cache.
- **Write Operation**: Simulates a write operation to the cache. It updates the cache with the new data and writes the data to main memory.
- **Cache Initialization**: Initializes the cache memory with the specified parameters such as sets, tag length, block size, and associativity.
- **LFU Replacement Policy**: Implements the Least Frequently Used (LFU) replacement policy to evict cache blocks when the cache is full.
- **Print Cache Contents**: Prints the contents of the cache memory for visualization and debugging purposes.

### Prerequisites

- C compiler (e.g., GCC)
- Standard C libraries

### Installation

1. Clone the repository to your local machine:

```
git clone https://github.com/your-username/cache-simulation.git
```

2. Compile  the source code:

```
cd cache-simulation
gcc -o cache_simulator main.c cache.c -lm
```

3. Running the Simulator

You can run the cache simulator by executing the compiled binary:
```
./cache_simulator
```


### Input
The simulator prompts the user to input the following parameters:

Size of data
Cache parameters: sets, tag length, block size, associativity
Memory access operations (read or write) along with the memory address and data value

### Output
The simulator displays the cache contents after each memory access operation. The cache contents include information about cache sets, cache lines, validity, frequency, tags, and block data.


### Examples:
INPUT:

Size of data: 8
Input data >> 1 2 3 4 5 6 7 8
s t b E: 1 1 1 2
0 1 2 6 7 -1

OUTPUT:

Set 0
1 2 0x0 01 02 
0 0 0x0 00 00 
Set 1
1 1 0x0 03 04 
1 2 0x1 07 08 


![image](https://github.com/ilanitb16/cache_simulator/assets/97344492/baa0fbd2-00eb-44f7-92c9-b96a923f6b64)

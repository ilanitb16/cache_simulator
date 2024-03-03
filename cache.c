#include <stdio.h>
#include <stdlib.h>
#include "cache.h"

#include <limits.h>

cache_t initialize_cache(uchar sets_exponent, uchar tag_length, uchar blocks_exponent, uchar E) {
    cache_t cache;
    cache.s = sets_exponent;
    cache.t = tag_length;
    cache.b = blocks_exponent;
    cache.E = E; // E is the number of lines per set

    int num_sets = 1 << sets_exponent; // 2^s, which is the number of sets

    //  sizeof(cache_line_t*) = size of pointer to a cache line.

    cache.cache = (cache_line_t**)malloc(num_sets * sizeof(cache_line_t*)); // allocate memory for array of sets.

    // for each cache set in the array
    for (int i = 0; i < num_sets; ++i) {
        // allocate memory E cache lines
        cache.cache[i] = (cache_line_t*)malloc(E * sizeof(cache_line_t));
        // for each cache line, initialize the values
        for (int j = 0; j < E; ++j) {
            cache.cache[i][j].valid = 0;
            cache.cache[i][j].frequency = 0;
            cache.cache[i][j].tag = 0;

            // allocate memory for block of memory cache line j of cache set i.
            // total size required to store the block of memory = 2^b
            cache.cache[i][j].block = (uchar*)malloc((1 << blocks_exponent) * sizeof(uchar));
        }
    }
    return cache;
}

void update_lfu(cache_line_t* cache_line, int cache_E, unsigned long int tag, long int off, uchar data, uchar b) {
    // Choose the cache line with the least frequency
    // Update the cache with the data we got from start
    int min_frequency = INT_MAX;
    int min_frequency_index = 0;

    // Iterate over each cache line and find minimum frequency
    for (int i = 0; i < cache_E; i++) {
        if (cache_line[i].frequency < min_frequency) {
            min_frequency = cache_line[i].frequency;
            min_frequency_index = i;
        }
    }

    // Update the cache line with the least frequently used one
    cache_line_t* least_freq_cache_line = &cache_line[min_frequency_index];
    least_freq_cache_line->valid = 1;
    least_freq_cache_line->frequency++;
    least_freq_cache_line->tag = tag;
    // least_freq_cache_line->block = (uchar*)malloc((1 << b) * sizeof(uchar));

    // Allocate memory for the block if it's not already allocated
    if (least_freq_cache_line->block == NULL) {
        least_freq_cache_line->block = (uchar*)malloc((1 << b) * sizeof(uchar));
    }
    least_freq_cache_line->block[off % (1 << b)] = data; // Update the block with the data
}

int check_cold_miss(cache_line_t* cache_line, int cache_E) {
    // Iterate over each cache line in the set
    for (int i = 0; i < cache_E; i++) {
        // If the cache line is not valid (empty)
        if (!cache_line[i].valid) {
            // Return true indicating a cold miss
            return 0;
        }
    }
    // Return false if all cache lines are valid (no cold miss)
    return 1;
}

uchar read_byte(cache_t cache, uchar* start, long int off) {
    // off is the address that you insert into your cache

    // Firstly we must determine if this memory reference causes a miss.

    // off >> (cache.s + cache.b) : shift memory address off to the right by cache.s + cache.b bits
   unsigned long int tag = off >> (cache.s + cache.b); //  Get the tag and the index from the address.

    // Determine the index for this memory reference. This determines
    // the set where the requested word *could* be.

    unsigned long int address_without_offset = off / (1 << cache.b); //div by size of block 2^b. left with index+tag bits
    unsigned long int index = address_without_offset % (1 << cache.s); // divide by size of set 2^s

    // Now that index and tag are known, determine if it is a hit/miss

    cache_line_t* cache_line = cache.cache[index]; // Search for the cache line matching the index
    for (int i = 0; i < cache.E; i++) {
        // If cache line is valid and its tag matches the extracted tag
        if (cache_line[i].valid && cache_line[i].tag == tag) {
            // Cache hit
            uchar cached_data = cache_line[i].block[off % (1 << cache.b)];
            cache_line[i].frequency++;

            // Return the data from the cache
            return cached_data;
        }
    }

    // If we got here we have a cache miss and have to handle it using LFU method.
    uchar data = start[off]; // Fetch the data from start (memory)
    uchar data2 = start[off + 1];

    // cold miss
    if ( 0 == check_cold_miss(cache_line, cache.E)) {
        // Find the first empty cache line and update it
        for (int i = 0; i < cache.E; i++) {
            if (!cache_line[i].valid) {
                cache_line[i].valid = 1;
                cache_line[i].frequency = 1;
                cache_line[i].tag = tag;
                cache_line[i].block = (uchar *) malloc((1 << cache.b) * sizeof(uchar));
                cache_line[i].block[off % (1 << cache.b)] = data;
                cache_line[i].block[(off + 1) % (1 << cache.b)] = data2;
                return data; // Return the fetched data
            }
        }
    }

    update_lfu(cache_line, cache.E, tag, off, data, cache.b); // Update cache using LFU method
    update_lfu(cache_line, cache.E, tag, off + 1, data2, cache.b);

    return data;
}

void write_byte(cache_t cache, uchar* start, long int off, uchar new) {
    // Implementation of write_byte function
    // Tag and index for the memory reference
    unsigned long int tag = off >> (cache.s + cache.b);
    unsigned long int address_without_offset = off / (1 << cache.b);
    unsigned long int index = address_without_offset % (1 << cache.s);

    // Search for the cache line matching the index
    cache_line_t* cache_line = cache.cache[index];
    for (int i = 0; i < cache.E; i++) {
        // If cache line is valid and its tag matches the extracted tag
        if (cache_line[i].valid && cache_line[i].tag == tag) {
            // Cache hit
            cache_line[i].frequency++;

            // Update the data in memory
            start[off] = new;
            return;
        }
    }

    // If the data is not in the cache, update the cache using the LFU method
    update_lfu(cache_line, cache.E, tag, off, new, cache.b);

    // Write the new data into memory
    start[off] = new;

}

void print_cache(cache_t cache) {
    int S = 1 << cache.s;
    int B = 1 << cache.b;

    for (int i = 0; i < S; i++) {
        printf("Set %d\n", i);
        for (int j = 0; j < cache.E; j++) {
            printf("%1d %d 0x%0*lx ", cache.cache[i][j].valid,
                   cache.cache[i][j].frequency, cache.t, cache.cache[i][j].tag);
            for (int k = 0; k < B; k++) {
                printf("%02x ", cache.cache[i][j].block[k]);
            }
            puts("");
        }
    }
}

int main (){
    // data
    uchar arr[] = {1, 2, 3, 4, 5, 6, 7, 8};
    cache_t cache = initialize_cache(1, 1, 1, 2);
    read_byte(cache, arr, 0);
    read_byte(cache, arr, 1);
    read_byte(cache, arr, 2);
    read_byte(cache, arr, 6);
    read_byte(cache, arr, 7);
    print_cache(cache);

//    int n;
//     printf("Size of data: ");
//     scanf("%d", &n);
//     uchar* mem = malloc(n);
//     printf("Input data >> ");
//     for (int i = 0; i < n; i++)
//         scanf("%hhd", mem + i);
//
//     int s, t, b, E;
//     printf("s t b E: ");
//     scanf("%d %d %d %d", &s, &t, &b, &E);
//     cache_t cache = initialize_cache(s, t, b, E);
//
//     while (1) {
//         scanf("%d", &n);
//         if (n < 0) break;
//         read_byte(cache, mem, n);
//         }
//
//     puts("");
//     print_cache(cache);
//
//     free(mem);

//    int n;
//    printf("Size of data: ");
//    scanf("%d", &n);
//    uchar* mem = malloc(n);
//
//    printf("Input data >> ");
//    for (int i = 0; i < n; i++)
//        scanf("%hhd", mem + i);
//
//    int s, t, b, E;
//    printf("s t b E: ");
//    scanf("%d %d %d %d", &s, &t, &b, &E);
//
//    cache_t cache = initialize_cache(s, t, b, E);
//
//    // Perform write operations
//    while (1) {
//        scanf("%d", &n);
//        if (n < 0) break;
//        write_byte(cache, mem, n, 10); // Write the value 10 at memory address n
//    }
//
//    printf("\nCache after write operations:\n");
//    print_cache(cache);
//
//    free(mem);
//    return 0;
}


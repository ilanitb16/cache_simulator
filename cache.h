#ifndef CACHE_H
#define CACHE_H

typedef unsigned char uchar; // represent byte values.

//  represents a cache line
typedef struct cache_line_s {
    uchar valid; //  validity bit
    uchar frequency; // frequency of use
    long int tag; // tag for line id
    uchar* block; // pointer to the block of memory
} cache_line_t;

// represents the cache
typedef struct cache_s {
    uchar s; // S = 2^s the number of sets
    uchar t; // t is the tag length
    uchar b; // B = 2^b the number of blocks per line in the set
    uchar E; // E is the number of lines per set
    cache_line_t** cache; // pointer to a pointer to cache_line_t.
} cache_t;

cache_t initialize_cache(uchar s, uchar t, uchar b, uchar E);
uchar read_byte(cache_t cache, uchar* start, long int off);
void write_byte(cache_t cache, uchar* start, long int off, uchar new);
void print_cache(cache_t cache);

// Ensures that if CACHE_H was not defined at the beginning of the file, it is now defined.
#endif /* CACHE_H */

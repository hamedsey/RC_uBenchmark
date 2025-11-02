#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <malloc.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <pthread.h> 
#include <sched.h>

//#include "pingpong.h"
#include <linux/types.h>  //for __be32 type
#include <errno.h>
#include <algorithm>
#include <random>

#include <x86intrin.h> // For __rdtsc()

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <array>

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <byteswap.h>


//g++ bitvector_overhead.cc -Wall -lm -lpthread -O3 -std=gnu++11 -o bv_overhead

#if __BYTE_ORDER == __LITTLE_ENDIAN
static inline uint64_t htonll(uint64_t x) { return bswap_64(x); }
static inline uint64_t ntohll(uint64_t x) { return bswap_64(x); }
#elif __BYTE_ORDER == __BIG_ENDIAN
static inline uint64_t htonll(uint64_t x) { return x; }
static inline uint64_t ntohll(uint64_t x) { return x; }
#else
#error __BYTE_ORDER is neither __LITTLE_ENDIAN nor __BIG_ENDIAN
#endif

volatile char * buf;



int main(int argc, char *argv[]) {
    
    uint64_t numberOfThreads = 1;
    uint64_t totalBytes;
    uint64_t thread_id = 0;
    uint64_t numConnections = 4096;//1024;//4096;
    unsigned long long leadingZeros = 0;
	uint64_t offset = (numConnections/numberOfThreads) * thread_id;
    uint64_t ii = 0;
    
    //totalBytes = numberOfQueues;//(numAllocatedBits/8)*numberOfThreads;
    totalBytes = 4096 * (numberOfThreads+1); //numberOfQueues;//(numAllocatedBits/8)*numberOfThreads;
    printf("number of threads = %lu \n", numberOfThreads);\
    printf("number of bytes allocated = %lu \n", totalBytes);
    
    //res->buf = (volatile char *)malloc(totalBytes * sizeof(volatile char));
    //assert(res->buf != NULL);

    buf = (volatile char *)memalign(offset, totalBytes * sizeof(volatile char));
    assert(buf != NULL);
    buf[(numConnections/8)-1] = 1;
    struct timespec start, end;

    uint64_t totalElapsed = 0;
    uint64_t iterations = 10000000;
    uint64_t warmUpIterations = 100000;
    /////////////////////////////////Multi Queue - lzcnt //////////////////////////////////////////////
    for(uint64_t i = 0; i < iterations; i++) {
        if(i < warmUpIterations) continue;
        clock_gettime(CLOCK_MONOTONIC,&start);
        while(1) {

            if(ii*8 >= numConnections) { //compare against upperbound for numConnections*8 to make code identical
                ii = 0;
                break;
            }

            unsigned long long value = htonll(*reinterpret_cast<volatile unsigned long long*>(buf + ii + (4096*thread_id)));

            while(value != 0) {

                __asm__ __volatile__ ("lzcnt %1, %0" : "=r" (leadingZeros) : "r" (value):);  //register allocated
                
                //printf("ii = %llu , value = %llu, leadingzeros = %llu \n", ii, (uint64_t)value, leadingZeros);
                value = value & (uint64_t)(0x7FFFFFFFFFFFFFFF >> leadingZeros);

                offset = (ii << 3) + leadingZeros;

            }
            //printf("ii = %llu , value = %llu, leadingzeros = %llu \n", ii, (uint64_t)value, leadingZeros);
            ii+=8;
        }
        clock_gettime(CLOCK_MONOTONIC,&end);
        uint64_t nsec = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
        totalElapsed += nsec;
        //printf("ii = %llu , value = %llu, leadingzeros = %llu \n", ii, (uint64_t)value, leadingZeros);
        //if(ii >= numConnections) ii = 0;
    }
    printf("nsec = %f \n", (float)(totalElapsed)/(float)(iterations-warmUpIterations));

}
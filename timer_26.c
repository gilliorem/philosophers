#include <time.h>
#include <stdio.h>

int main() {
    struct timespec start, end;
    // CLOCK_MONOTONIC is immune to system time changes
    clock_gettime(CLOCK_MONOTONIC, &start);

    // ... code to time ...

    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate elapsed time in milliseconds
    double elapsed = (end.tv_sec - start.tv_sec) * 1000.0 + 
                     (end.tv_nsec - start.tv_nsec) / 1000000.0;
    
    printf("Elapsed: %.3f ms\n", elapsed);
}


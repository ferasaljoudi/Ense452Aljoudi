#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#ifndef PI
#define PI 3.14159265358979
#endif

#define DEGREE_RESOLUTION 0.5
#define SCALE 100000
#define TABLE_SIZE 1440
#define NUM_LOOPS 1000000000 // Iterations

int sin_table[TABLE_SIZE];

void sin_lut()
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        double angle = (i - (TABLE_SIZE / 2)) * DEGREE_RESOLUTION;
        sin_table[i] = (int)(sin(angle * PI / 180.0) * SCALE);
    }
}

int sin_value(int angle_index)
{
    return sin_table[angle_index];
}

long long time_in_ns(struct timespec start, struct timespec end)
{
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

int main()
{
    struct timespec start, end;

    sin_lut();

    int angle_index = 720; // Index 720 represents 0 degrees

    // Timing for LUT
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (long i = 0; i < NUM_LOOPS; i++)
    {
        int lut_value = sin_value(angle_index);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    long long table_lookup = time_in_ns(start, end);
    printf("Total time for table_lookup: %lld ns\n", table_lookup);

    // Timing for standard sin() function
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (long i = 0; i < NUM_LOOPS; i++)
    {
        double std_value = sin(0.5 * PI / 180.0) * SCALE;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    long long total_time_std = time_in_ns(start, end);
    printf("Total time for sin(): %lld ns\n", total_time_std);

    double speedup = (double)total_time_std / (double)table_lookup;
    printf("Table Speedup: %.4fx\n", speedup);

    return 0;
}

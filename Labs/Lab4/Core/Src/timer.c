#include "timer.h"
#include "stm32f1xx_hal.h"
#include "cli.h"
#include <stdlib.h>
#include <stdio.h>

extern TIM_HandleTypeDef htim2;

#define TRIALS 100

void timer_init(void) {
    HAL_TIM_Base_Start(&htim2);
}

uint16_t timer_start(void) {
    return __HAL_TIM_GET_COUNTER(&htim2);
}

uint16_t timer_stop(uint16_t start_time) {
    uint16_t end_time = __HAL_TIM_GET_COUNTER(&htim2);
    if (end_time >= start_time) {
        // No wraparound
        return end_time - start_time;
    } else {
        // Handle single wraparound from 0xFFFF to 0x0000
        return (0xFFFF - start_time + end_time + 1);
    }
}

// Arrays to hold pre-generated random values
static int32_t rand32_a[TRIALS], rand32_b[TRIALS];
static int64_t rand64_a[TRIALS], rand64_b[TRIALS];

// Function to generate random values outside of the measurement loop
static void generate_random_values() {
    for (int i = 0; i < TRIALS; i++) {
        rand32_a[i] = rand();
        rand32_b[i] = rand();
        rand64_a[i] = ((int64_t)rand() << 32) | rand();
        rand64_b[i] = ((int64_t)rand() << 32) | rand();
    }
}

// Operations to measure using pre-generated random values
static void add_32bit(int trial) {
    volatile int32_t c = rand32_a[trial] + rand32_b[trial];
}

static void add_64bit(int trial) {
    volatile int64_t c = rand64_a[trial] + rand64_b[trial];
}

static void multiply_32bit(int trial) {
    volatile int32_t c = rand32_a[trial] * rand32_b[trial];
}

static void multiply_64bit(int trial) {
    volatile int64_t c = rand64_a[trial] * rand64_b[trial];
}

static void divide_32bit(int trial) {
    volatile int32_t c = (rand32_a[trial] != 0) ? rand32_b[trial] / rand32_a[trial] : 0;  // Avoiding division by zero
}

static void divide_64bit(int trial) {
    volatile int64_t c = (rand64_a[trial] != 0) ? rand64_b[trial] / rand64_a[trial] : 0;  // Avoiding division by zero
}

// Structures for copy operations
typedef struct {
    uint64_t data[1]; // 8 bytes
} struct8_t;

typedef struct {
    uint64_t data[16]; // 128 bytes
} struct128_t;

typedef struct {
    uint64_t data[128]; // 1024 bytes
} struct1024_t;

// Copy operations for structs
static void copy_8byte_struct(int trial) {
    struct8_t src = {{0}};
    struct8_t dst;
    dst = src;
}

static void copy_128byte_struct(int trial) {
    struct128_t src = {{0}};
    struct128_t dst;
    dst = src;
}

static void copy_1024byte_struct(int trial) {
    struct1024_t src = {{0}};
    struct1024_t dst;
    dst = src;
}

// Function to measure the time taken by an operation over 100 trials
static uint16_t measure_time(void (*operation)(int)) {
    uint16_t start = timer_start();
    for (int i = 0; i < TRIALS; i++) {
        operation(i);
    }
    return timer_stop(start) / TRIALS;
}

void run_timing_tests(void) {
    char buffer[100];

    // Generating random values for all operations
    generate_random_values();

    uint16_t result_32bit_add = measure_time(add_32bit);
    uint16_t result_64bit_add = measure_time(add_64bit);
    uint16_t result_32bit_mul = measure_time(multiply_32bit);
    uint16_t result_64bit_mul = measure_time(multiply_64bit);
    uint16_t result_32bit_div = measure_time(divide_32bit);
    uint16_t result_64bit_div = measure_time(divide_64bit);
    uint16_t result_copy_8byte = measure_time(copy_8byte_struct);
    uint16_t result_copy_128byte = measure_time(copy_128byte_struct);
    uint16_t result_copy_1024byte = measure_time(copy_1024byte_struct);

    cli_print("You asked for the timing test:\r\n");

    cli_print("\x1b[33m");

    sprintf(buffer, "32-bit Addition: %d ticks\r\n", result_32bit_add);
    cli_print(buffer);

    sprintf(buffer, "64-bit Addition: %d ticks\r\n", result_64bit_add);
    cli_print(buffer);

    sprintf(buffer, "32-bit Multiplication: %d ticks\r\n", result_32bit_mul);
    cli_print(buffer);

    sprintf(buffer, "64-bit Multiplication: %d ticks\r\n", result_64bit_mul);
    cli_print(buffer);

    sprintf(buffer, "32-bit Division: %d ticks\r\n", result_32bit_div);
    cli_print(buffer);

    sprintf(buffer, "64-bit Division: %d ticks\r\n", result_64bit_div);
    cli_print(buffer);

    sprintf(buffer, "8-byte Struct Copy: %d ticks\r\n", result_copy_8byte);
	cli_print(buffer);

	sprintf(buffer, "128-byte Struct Copy: %d ticks\r\n", result_copy_128byte);
	cli_print(buffer);

	sprintf(buffer, "1024-byte Struct Copy: %d ticks\r\n", result_copy_1024byte);
	cli_print(buffer);

	cli_print("\x1b[0m");
}

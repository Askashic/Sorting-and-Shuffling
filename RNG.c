#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// PCG32 structure
typedef struct {
    uint64_t state;
    uint64_t inc;
} pcg32_random_t;

// PCG32 function
uint32_t pcg32_random_r(pcg32_random_t* rng) {
    uint64_t oldstate = rng->state;
    rng->state = oldstate*6364136223846793005ULL+rng->inc;
    uint32_t xorshifted = ((oldstate>>18u)^oldstate)>>27u;
    uint32_t rot = oldstate>>59u;
    return (xorshifted>>rot)|(xorshifted<<((-rot)&31));
}

void pcg32_srandom_r(pcg32_random_t* rng, uint64_t seed, uint64_t stream) {
    rng->state = 0U;
    rng->inc = (stream<<1u)|1u;
    pcg32_random_r(rng);
    rng->state += seed;
    pcg32_random_r(rng);
}

// Array structure
typedef struct {
    uint32_t* data;
    int size;
} random_array_t;

// Thread Data Structure
typedef struct {
    int thread_id;
    random_array_t array;  // Array structure
    int generate_size;     // The array' s size
} thread_data_t;

// Generate random bits 0/1
uint32_t generate_random_bit(pcg32_random_t* rng) {
    uint32_t random_num = pcg32_random_r(rng);
    return random_num & 1;
}

// Thread function
void* thread_func(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;

    // Initialise RNG
    pcg32_random_t rng;
    uint64_t seed = time(NULL) ^ (data->thread_id << 16);
    pcg32_srandom_r(&rng, seed, data->thread_id);

    // Allocate memory for the array
    data->array.data = (uint32_t*)malloc(data->generate_size * sizeof(uint32_t));
    if (data->array.data == NULL) {
        fprintf(stderr, "Thread %d: fails\n", data->thread_id);
        return NULL;
    }
    data->array.size = data->generate_size;

    // Generate the array
    for (int i = 0; i < data->array.size; i++) {
        data->array.data[i] = generate_random_bit(&rng);
    }

    printf("Thread %d: Have generated %d arrays\n", data->thread_id, data->array.size);

    return NULL;
}

// Print the array
void print_array(const random_array_t* array, int thread_id, int items_per_line) {
    printf("\nThread %d 's array (%d elements):\n", thread_id, array->size);

    for (int i = 0; i < array->size; i++) {
        printf("[%3d] %6u", i, array->data[i]);

        if ((i + 1) % items_per_line == 0) {
            printf("\n");
        } else if (i < array->size - 1) {
            printf("  ");
        }
    }

    if (array->size % items_per_line != 0) {
        printf("\n");
    }
}

int main() {
    const int NUM_THREADS = 2;
    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];

    // The array' s size of each thread
    int sizes[] = {8, 8};

    printf("=== Start ===\n\n");

    // Initialise thread data
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].generate_size = sizes[i];
        thread_data[i].array.data = NULL;
        thread_data[i].array.size = 0;
    }

    // Create and run threads
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, thread_func, &thread_data[i]) != 0) {
            fprintf(stderr, "Build thread %d fails\n", i);
            return 1;
        }
    }

    // Waiting for the thread to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print all arrays
    printf("\n=== The array ===\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        print_array(&thread_data[i].array, i, 4);
    }

    // Release memory
    for (int i = 0; i < NUM_THREADS; i++) {
        free(thread_data[i].array.data);
    }

    printf("\nEnd\n");
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

// Swap function
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Fisher-Yates
void FY( int c[], int n) {
    srand(time(NULL));
    for(int i = n-1; i >= 0; i--) {
        int j = rand() % (i+1);
        swap(&c[i], &c[j]);
    }
}

// Rao-Sandelius
void Recursive(int c[], int left, int right) {
    if (left >= right) return; // When length <= 1, remain the same

    int p = left+rand()%(right-left+1); // Choose a random element to swap with the 1st one
    swap(&c[p], &c[left]);

    int i = left+1; // Start with the second element
    int j = right;

    while (i <= j) {
        if (rand()%2 == 0) { // Keep the 0-group in front of the 1-group
            i++;
        } else {
            swap(&c[i], &c[j]);
            j--;
        }
    }

    swap(&c[left], &c[j]);

    Recursive(c, left, j - 1);
    Recursive(c, j + 1, right);
}

void RS(int c[], int n) {
    Recursive(c, 0, n - 1);
}

// time count
// The CLOCK() function had insufficient precision, so we employed this one as a replacement
#ifdef __linux__
#include <sys/time.h>
double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}
#else
double get_time() {
    return (double)clock() / CLOCKS_PER_SEC;
}
#endif

// Testing algorithm performance (averaging multiple runs)
double test_in_high_precision(void (*func)(int[], int), int arr[], int n, int iterations) {
    int* test_arr = (int*)malloc(n*sizeof(int));
    if (test_arr == NULL) {
        printf("error !\n");
        return -1;
    }

    double start_time = get_time();

    for (int iter = 0; iter < iterations; iter++) {
        memcpy(test_arr, arr, n*sizeof(int));
        func(test_arr, n);
    }

    double end_time = get_time();
    double total_time = (end_time-start_time)*1000.0; // 转换为毫秒
    double avg_time = total_time/iterations;

    free(test_arr);
    return avg_time;
}

// Time usd in high precision
void run_benchmark_high_precision() {
    printf("Fisher-Yates vs Rao-Sandelius in high precision\n");
    printf("===================================================\n");

    int sizes[] = {100, 500, 1000, 2000, 5000, 10000};
    int num_sizes = sizeof(sizes)/sizeof(sizes[0]);
    int iterations[] = {10000, 5000, 2000, 1000, 500, 100};

    printf("\nsize\tFisher-Yates(ms)\tRao-Sandelius(ms)\tratio\n");
    printf("----------------------------------------------------------------\n");

    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        int iter = iterations[i];

        int* test_array = (int*)malloc(n * sizeof(int));
        if (test_array == NULL) {
            printf("error !\n");
            continue;
        }

        for (int j = 0; j < n; j++) {
            test_array[j] = j;
        }

        // Test Fisher-Yates
        double fy_time = test_in_high_precision(FY, test_array, n, iter);

        // Test Rao-Sandelius
        double rs_time = test_in_high_precision(RS, test_array, n, iter);

        double ratio = rs_time/fy_time;

        printf("%d\t%.6f\t\t%.6f\t\t%.2fx\n", n, fy_time, rs_time, ratio);

        free(test_array);
    }
}

// Time used in small arrays
void run_benchmark_small_array() {
    printf("Fisher-Yates vs Rao-Sandelius in small array\n");
    printf("===================================================\n");

    int small_sizes[] = {10, 50, 100, 200};
    int num_sizes = sizeof(small_sizes)/sizeof(small_sizes[0]);
    int iterations = 100000;

    printf("size\tFisher-Yates(ms)\tRao-Sandelius(ms)\tratio\n");
    printf("----------------------------------------------------------------\n");

    for (int i = 0; i < num_sizes; i++) {
        int n = small_sizes[i];

        int* test_array = (int*)malloc(n * sizeof(int));
        if (test_array == NULL) {
            printf("error !\n");
            continue;
        }

        for (int j = 0; j < n; j++) {
            test_array[j] = j;
        }

        double fy_time = test_in_high_precision(FY, test_array, n, iterations);
        double rs_time = test_in_high_precision(RS, test_array, n, iterations);

        double ratio = rs_time/fy_time;

        printf("%d\t%.6f\t\t%.6f\t\t%.2fx\n", n, fy_time, rs_time, ratio);

        free(test_array);
    }
}

int main() {
    srand((unsigned int)time(NULL));

    printf("Time benchmark\n");

    // Small array
    run_benchmark_small_array();
    printf("\n");

    // High precisiom
    run_benchmark_high_precision();

    printf("\nend\n");

    return 0;
}

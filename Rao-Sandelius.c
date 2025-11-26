#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// The function used
void RS(int *c, int l);
void swap(int *a, int *b);
void printArray(int *arr, int l, const char *name);

int main() {
    // Initialise the random number seed
    srand(time(NULL));

    // The array c
    int c[] = {1, 2, 3, 4, 5, 6};
    int l = sizeof(c) / sizeof(c[0]);  // The array' s length

    printArray(c, l, "The original array:");

    // Use the RS function
    RS(c, l);

    printArray(c, l, "The new array:");

    return 0;
}

// Swap 2 elements
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Print the array
void printArray(int *arr, int l, const char *name) {
    printf(name, l);
    for(int i = 0; i < l; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

// RS function
void RS(int *c, int l) {

    // Make sure the length is correct
    if (l <= 0) return;
    
    // Case 1：The array' s length is 1
    if(l == 1) {
        return; // Just return the original one
    }

    // Case 2：The length is 2
    if(l == 2) {
        int bit = rand() % 2;  // Generate a rand bit

        if(bit == 0) { // If rand bit is 0, return the original one
            return;
        } else if(bit == 1) {
            swap(&c[0], &c[1]);  // If rand bit is 1, swap the 2 elements
            return;
        }
    }

    // Case 3: The length > 2
    // Allocate memory for the bit array
    int *Bit = (int*)malloc(l * sizeof(int));

    // Initialise the bit array
    for(int i = 0; i < l; i++) {
        Bit[i] = 0;
    }

    // The first element is randomly generated as either 0 or 1
    Bit[0] = rand() % 2;

    int zero_count = 1;  // Indexing starting from the first element

    // Main loop (starting from the second element)
    for(int i = 1; i < l; i++) {
        int bit = rand() % 2;  // Generate a random bit
        Bit[i] = bit;

        // (0, 0)
        if(Bit[i-1] == 0 && Bit[i] == 0) {
            zero_count = zero_count + 1;
        }

        // (0, 1)
        if(Bit[i-1] == 0 && Bit[i] == 1) {
            zero_count = zero_count + 1;
        }

        // (1, 0)
        if(Bit[i-1] == 1 && Bit[i] == 0) {

            // Swap Bit[i] and Bit[zero_count-1]
            swap(&Bit[i], &Bit[zero_count - 1]);

            // Swap c[i] and c[zero_count-1]
            swap(&c[i], &c[zero_count - 1]);

            zero_count = zero_count + 1;
        }
    }

    // Repeat the above steps until length = 1 or 2
    // 0-group: 1:zero_count-1
    RS(c, zero_count - 1);

    // 1-group: = zero_count:l
    RS(c + zero_count - 1, l - zero_count + 1);

    // Release the memory allocated to the bit array
    free(Bit);

}



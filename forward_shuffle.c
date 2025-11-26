#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void forward_shuffle(int c[], int n) {
    srand(time(NULL));

    for (int i = 0; i < n - 1; i++) {
        int j = i + rand() % (n - i);

        swap(&c[i], &c[j]);;
    }
}

int main() {
    int n = 7;
    int c[] = {1, 2, 3, 4, 5, 6, 7};

    printf("The original array:");
    for(int i = 0; i < n; i++) {
        printf("%d ", c[i]);
    }
    printf("\n");

    forward_shuffle(c, n);
    printf("The new array:");
    for(int i = 0; i < n; i++) {
        printf("%d ", c[i]);
    }
    printf("\n");

    return 0;
}

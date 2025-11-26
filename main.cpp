#include <cstdio>
#include <cstdlib>
#include <ctime>

void FY(int n, int c[]) {
    srand(time(NULL));
    for(int i = n-1; i >= 0; i--) {
        int j = rand() % (i+1);
        int t = c[i];
        c[i] = c[j];
        c[j] = t;
    }
}

int main() {
    int c[] = {3, 4, 5, 1, 7, 8, 2, 10};
    int n = sizeof(c) / sizeof(c[0]);

    printf("The original array:");
    for(int i = 0; i < n; i++) {
        printf("%d ", c[i]);
    }
    printf("\n");

    FY(n, c);
    printf("The new array:");
    for(int i = 0; i < n; i++) {
        printf("%d ", c[i]);
    }
    printf("\n");
    return 0;
}
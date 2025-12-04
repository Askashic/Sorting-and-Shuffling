#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>

// PCG32状态结构
typedef struct {
    uint64_t state;
    uint64_t increment;
    const char* name;
} pcg32_t;

// 线程局部存储的PCG状态
static __thread pcg32_t pcg_state;

// 初始化PCG32
void pcg32_init(pcg32_t* rng, uint64_t seed, uint64_t stream, const char* name) {
    rng->state = 0U;
    rng->increment = (stream << 1u) | 1u;
    rng->name = name;
}

// PCG32核心随机数生成
uint32_t pcg32_random(pcg32_t* rng) {
    uint64_t oldstate = rng->state;

    // LCG步骤：state = state * multiplier + increment
    rng->state = oldstate * 6364136223846793005ULL + rng->increment;

    // 输出函数：XORSHIFT + 旋转
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;

    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

// 生成指定范围的随机整数
uint32_t pcg32_bounded_rand(pcg32_t* rng, uint32_t bound) {
    // 拒绝采样方法，确保均匀分布
    uint32_t threshold = -bound % bound;
    uint32_t r;

    do {
        r = pcg32_random(rng);
    } while (r < threshold);

    return r % bound;
}

// 生成[0,1)范围的随机浮点数
double pcg32_random_double(pcg32_t* rng) {
    return (double)pcg32_random(rng) / (double)0x100000000ULL;
}

// 生成[min, max]范围的随机整数
int pcg32_range(pcg32_t* rng, int min, int max) {
    return min + pcg32_bounded_rand(rng, max - min + 1);
}

void* thread_func(void* arg) {
    int thread_id = *(int*)arg;

    // 为每个线程生成唯一的种子和流标识符
    uint64_t seed = time(NULL) + thread_id * 1000;
    uint64_t stream = (uint64_t)pthread_self();

    char name[32];
    snprintf(name, sizeof(name), "Thread-%d", thread_id);

    // 初始化线程局部的PCG
    pcg32_init(&pcg_state, seed, stream, name);

    printf("%s: Seed=%lu, Stream=%lu\n",
           pcg_state.name, seed, stream);

    // 生成多种类型的随机数
    for (int i = 0; i < 6; i++) {
        uint32_t rand32 = pcg32_random(&pcg_state);
        double rand_double = pcg32_random_double(&pcg_state);
        int rand_range = pcg32_range(&pcg_state, 1, 100);

        printf("%s - Int: %10u, Double: %.6f, Range[1-100]: %3d\n",
               pcg_state.name, rand32, rand_double, rand_range);
    }

    return NULL;
}

// 测试PCG的统计特性
void test_pcg_statistics() {
    pcg32_t rng;
    pcg32_init(&rng, 42, 54, "Test-RNG");

    int buckets[10] = {0};
    const int samples = 100000;

    // 测试均匀分布
    for (int i = 0; i < samples; i++) {
        double r = pcg32_random_double(&rng);
        int bucket = (int)(r * 10);
        buckets[bucket]++;
    }

    printf("\nPCG distribution test (%d sample):\n", samples);
    for (int i = 0; i < 10; i++) {
        double percent = (double)buckets[i] / samples * 100;
        printf("Bucket [%.1f-%.1f): %5d (%.1f%%)\n",
               i/10.0, (i+1)/10.0, buckets[i], percent);
    }
}

int main() {
    pthread_t threads[4];
    int thread_ids[4] = {1, 2, 3, 4};

    printf("PCG RNG\n");
    printf("=========================\n");

    // 创建多个线程
    for (int i = 0; i < 4; i++) {
        if (pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // 等待所有线程完成
    for (int i = 0; i < 4; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
            return 1;
        }
    }

    printf("\n");
    test_pcg_statistics();

    return 0;
}
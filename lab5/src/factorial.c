#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <getopt.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct FactorialArgs {
    int start;
    int end;
    int mod;
};

void *CalculateFactorial(void *args) {
    struct FactorialArgs *fargs = (struct FactorialArgs *)args;
    int start = fargs->start;
    int end = fargs->end;
    int mod = fargs->mod;
    int *partial_result = (int *)malloc(sizeof(int));
    *partial_result = 1;

    printf("Thread %ld: partial result initialized to %d\n", pthread_self(), *partial_result);

    for (int i = start; i <= end; i++) {
        *partial_result = (*partial_result * (i % mod)) % mod;
    }

    printf("Thread %ld result: %d\n", pthread_self(), *partial_result);
    pthread_exit((void *)partial_result);
}

int main(int argc, char **argv) {
    int k = -1;
    int pnum = -1;
    int mod = -1;

    while (1) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {{"k", required_argument, 0, 0},
                                          {"pnum", required_argument, 0, 0},
                                          {"mod", required_argument, 0, 0},
                                          {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "k:pnum:mod:", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0: {
                switch (option_index) {
                    case 0:
                        k = atoi(optarg);
                        if (k <= 0) {
                            printf("k must be a positive number\n");
                            return 1;
                        }
                        break;
                    case 1:
                        pnum = atoi(optarg);
                        if (pnum <= 0) {
                            printf("pnum must be a positive number\n");
                            return 1;
                        }
                        break;
                    case 2:
                        mod = atoi(optarg);
                        if (mod <= 0) {
                            printf("mod must be a positive number\n");
                            return 1;
                        }
                        break;
                    default:
                        printf("Index %d is out of options\n", option_index);
                }
            } break;

            case '?':
                printf("Unknown argument\n");
                break;
            default:
                fprintf(stderr, "getopt returned character code 0%o?\n", c);
        }
    }

    if (k == -1 || pnum == -1 || mod == -1) {
        printf("Usage: %s --k \"num\" --pnum \"num\" --mod \"num\"\n", argv[0]);
        return 1;
    }

    printf("k = %d, pnum = %d, mod = %d\n", k, pnum, mod);

    pthread_t threads[pnum];
    struct FactorialArgs args[pnum];

    int step = k / pnum;

    for (int i = 0; i < pnum; i++) {
        args[i].start = i * step + 1;
        args[i].end = (i == pnum - 1) ? k : (i + 1) * step;
        args[i].mod = mod;
        if (pthread_create(&threads[i], NULL, CalculateFactorial, (void *)&args[i])) {
            perror("pthread_create");
            exit(1);
        }
        printf("Thread %ld: start = %d, end = %d\n", threads[i], args[i].start, args[i].end);
    }

    int total_result = 1;
    for (int i = 0; i < pnum; i++) {
        int *result;
        if (pthread_join(threads[i], (void **)&result) != 0) {
            perror("pthread_join");
            exit(1);
        }
        pthread_mutex_lock(&mutex);
        total_result = (total_result * (*result)) % mod;
        pthread_mutex_unlock(&mutex);
        printf("Thread %ld result: %d\n", threads[i], *result);
        free(result);
    }

    printf("%d! mod %d = %d\n", k, mod, total_result);

    return 0;
}

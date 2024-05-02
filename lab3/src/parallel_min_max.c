#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>
#include <signal.h>

#include "find_min_max.h"
#include "utils.h"

// Глобальная переменная для хранения PID дочернего процесса
pid_t child_pid;

// Обработчик сигнала SIGALRM
void handle_alarm(int sig) {
    // Отправляем сигнал SIGKILL дочернему процессу
    kill(child_pid, SIGKILL);
}

int main(int argc, char **argv) {
    int seed = -1;
    int array_size = -1;
    int pnum = -1;
    bool with_files = false;
    int timeout = -1; // Переменная для хранения таймаута

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {"by_files", no_argument, 0, 'f'},
            {"timeout", required_argument, 0, 0}, // Добавляем опцию для установки таймаута
            {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);

        if (c == -1)
            break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        seed = atoi(optarg);
                        break;
                    case 1:
                        array_size = atoi(optarg);
                        break;
                    case 2:
                        pnum = atoi(optarg);
                        break;
                    case 3:
                        with_files = true;
                        break;
                    case 4:
                        timeout = atoi(optarg); // Считываем значение таймаута из аргумента командной строки
                        break;
                    default:
                        printf("Index %d is out of options\n", option_index);
                }
                break;
            case 'f':
                with_files = true;
                break;
            case '?':
                break;
            default:
                printf("getopt returned character code 0%o?\n", c);
        }
    }

    if (optind < argc) {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (seed == -1 || array_size == -1 || pnum == -1) {
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout \"num\"]\n", argv[0]);
        return 1;
    }

    // Если таймаут задан, устанавливаем обработчик сигнала SIGALRM
    if (timeout > 0) {
        signal(SIGALRM, handle_alarm);
        // Устанавливаем таймер сигнала
        alarm(timeout);
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    int active_child_processes = 0;

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    int pipefd[2];
    if (!with_files) {
        if (pipe(pipefd) == -1) {
            perror("pipe");
            return 1;
        }
    }

    for (int i = 0; i < pnum; i++) {
        pid_t child_pid = fork();
        if (child_pid >= 0) {
            active_child_processes += 1;
            if (child_pid == 0) {
                struct MinMax min_max;
                if (i == pnum - 1) {
                    min_max = GetMinMax(array, i * array_size / pnum, array_size);
                } else {
                    min_max = GetMinMax(array, i * array_size / pnum, (i + 1) * array_size / pnum);
                }

                if (with_files) {
                    FILE *fp;
                    fp = fopen("temp.txt", "w+");
                    fprintf(fp, "%d %d\n", min_max.min, min_max.max);
                    fclose(fp);
                } else {
                    close(pipefd[0]);
                    write(pipefd[1], &min_max, sizeof(struct MinMax));
                    close(pipefd[1]);
                }
                return 0;
            }

        } else {
            printf("Fork failed!\n");
            return 1;
        }
    }

    while (active_child_processes > 0) {
        int status;
        wait(&status);
        active_child_processes -= 1;
    }

    struct MinMax min_max;
    min_max.min = INT_MAX;
    min_max.max = INT_MIN;

    for (int i = 0; i < pnum; i++) {
        int min = INT_MAX;
        int max = INT_MIN;

        if (with_files) {
            FILE *fp;
            fp = fopen("temp.txt", "r");
            fscanf(fp, "%d %d", &min, &max);
            fclose(fp);
        } else {
            struct MinMax received;
            close(pipefd[1]);
            read(pipefd[0], &received, sizeof(struct MinMax));
            close(pipefd[0]);
            min = received.min;
            max = received.max;
        }

        if (min < min_max.min) min_max.min = min;
        if (max > min_max.max) min_max.max = max;
    }

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);

    printf("Min: %d\n", min_max.min);
    printf("Max: %d\n", min_max.max);
    printf("Elapsed time: %fms\n", elapsed_time);
    fflush(NULL);
    return 0;
}

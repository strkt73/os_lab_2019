#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "utils.h" // Подключаем библиотеку для генерации массива

struct SumArgs {
  int *array;
  int begin;
  int end;
};

int Sum(const struct SumArgs *args) {
  int sum = 0;
  for (int i = args->begin; i < args->end; i++) {
    sum += args->array[i];
  }
  return sum;
}

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  if (argc != 7) {
    printf("Usage: %s --threads_num <num> --seed <num> --array_size <num>\n", argv[0]);
    return 1;
  }

  uint32_t threads_num = atoi(argv[2]);
  uint32_t seed = atoi(argv[4]);
  uint32_t array_size = atoi(argv[6]);

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed); // Генерируем массив

  pthread_t threads[threads_num];
  struct SumArgs args[threads_num];
  int step = array_size / threads_num;
  for (uint32_t i = 0; i < threads_num; i++) {
    args[i].array = array;
    args[i].begin = i * step;
    args[i].end = (i == threads_num - 1) ? array_size : (i + 1) * step;
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  int total_sum = 0;
  void *result;
  for (uint32_t i = 0; i < threads_num; i++) {
    if (pthread_join(threads[i], &result)) {
      printf("Error: pthread_join failed!\n");
      return 1;
    }
    total_sum += (int)(size_t)result;
  }

  free(array);
  printf("Total: %d\n", total_sum);
  return 0;
}

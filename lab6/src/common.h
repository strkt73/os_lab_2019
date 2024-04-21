#ifndef COMMON_H
#define COMMON_H
#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// объявление структуры Server
struct Server {
  char ip[255];
  int port;
};

// функция для умножения по модулю
uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod);

// функция для преобразования строки в uint64_t
bool ConvertStringToUI64(const char *str, uint64_t *val);

// структура для передачи аргументов в поток
struct FactorialArgs {
  uint64_t begin;
  uint64_t end;
  uint64_t mod;
};

uint64_t Factorial(const struct FactorialArgs *args);

#endif

#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

void *ThreadFactorial(void *args) {
  struct FactorialArgs *fargs = (struct FactorialArgs *)args;
  return (void *)(uint64_t *)Factorial(fargs);
}

void *ThreadHandleRequest(void *thread_args) {
  int client_fd = *((int *)thread_args);
  int tnum = *((int *)thread_args + 1);
  free(thread_args);

  while (true) {
    unsigned int buffer_size = sizeof(uint64_t) * 3;
    char from_client[buffer_size];
    int read = recv(client_fd, from_client, buffer_size, 0);
    if (!read)
      break;
    if (read < 0) {
      fprintf(stderr, "Client read failed\n");
      break;
    }
    if (read < buffer_size) {
      fprintf(stderr, "Client send wrong data format\n");
      break;
    }

    pthread_t threads[tnum];
    uint64_t begin = 0;
    uint64_t end = 0;
    uint64_t mod = 0;
    memcpy(&begin, from_client, sizeof(uint64_t));
    memcpy(&end, from_client + sizeof(uint64_t), sizeof(uint64_t));
    memcpy(&mod, from_client + 2 * sizeof(uint64_t), sizeof(uint64_t));
    fprintf(stdout, "Receive: %llu %llu %llu\n", begin, end, mod);

    struct FactorialArgs args[tnum];
    for (uint32_t i = 0; i < tnum; i++) {
      args[i].begin = begin;
      args[i].end = end;
      args[i].mod = mod;
      if (pthread_create(&threads[i], NULL, ThreadFactorial, (void *)&args[i])) {
        printf("Error: pthread_create failed!\n");
        return NULL;
      }
    }

    uint64_t total = 1;
    for (uint32_t i = 0; i < tnum; i++) {
      uint64_t result = 0;
      pthread_join(threads[i], (void **)&result);
      total = MultModulo(total, result, mod);
    }

    printf("Total: %llu\n", total);
    char buffer[sizeof(total)];
    memcpy(buffer, &total, sizeof(total));
    int err = send(client_fd, buffer, sizeof(total), 0);
    if (err < 0) {
      fprintf(stderr, "Can't send data to client\n");
      break;
    }
  }

  close(client_fd);
  return NULL;
}

int main(int argc, char **argv) {
  int tnum = -1;
  int port = -1;
  while (true) {
    int current_optind = optind ? optind : 1;
    static struct option options[] = {{"port", required_argument, 0, 0},
                                      {"tnum", required_argument, 0, 0},
                                      {0, 0, 0, 0}};
    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);
    if (c == -1)
      break;
    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        port = atoi(optarg);
        break;
      case 1:
        tnum = atoi(optarg);
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
  if (port == -1 || tnum == -1) {
    fprintf(stderr, "Using: %s --port 20001 --tnum 4\n", argv[0]);
    return 1;
  }

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    fprintf(stderr, "Can not create server socket!");
    return 1;
  }
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons((uint16_t)port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  int opt_val = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
  int err = bind(server_fd, (struct sockaddr *)&server, sizeof(server));
  if (err < 0) {
    fprintf(stderr, "Can not bind to socket!");
    return 1;
  }
  err = listen(server_fd, 128);
  if (err < 0) {
    fprintf(stderr, "Could not listen on socket\n");
    return 1;
  }
  printf("Server listening at %d\n", port);

  while (true) {
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);
    if (client_fd < 0) {
      fprintf(stderr, "Could not establish new connection\n");
      continue;
    }

    // создаем поток для обработки запроса
    pthread_t thread;
    int *args = (int *)malloc(sizeof(int) * 2); 
    args[0] = client_fd; // файловый дескриптор клиента
    args[1] = tnum; // количество потоков для факториала

    // поток для обработки запроса
    if (pthread_create(&thread, NULL, ThreadHandleRequest, (void *)args) != 0) {
      fprintf(stderr, "Error creating thread\n");
      exit(1);
    }
  }

  return 0;
}

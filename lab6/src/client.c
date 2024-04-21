#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'};
  while (true) {
    int current_optind = optind ? optind : 1;
    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};
    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);
    if (c == -1)
      break;
    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        ConvertStringToUI64(optarg, &k);
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        break;
      case 2:
        memcpy(servers, optarg, strlen(optarg));
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;
    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }
  if (k == -1 || mod == -1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    return 1;
  }

  FILE *file = fopen(servers, "r");
  if (!file) {
    perror("fopen");
    return 1;
  }

  // количество серверов
  unsigned int servers_num = 0;
  char line[255];
  while (fgets(line, sizeof(line), file)) {
    servers_num++;
  }
  rewind(file); // возврат к началу файла

  struct Server *to = malloc(sizeof(struct Server) * servers_num);
  if (!to) {
    perror("malloc");
    fclose(file);
    return 1;
  }

  // чтение серверов из файла
  for (int i = 0; i < servers_num; i++) {
    if (fscanf(file, "%s", to[i].ip) != 1) {
      perror("fscanf");
      fclose(file);
      free(to);
      return 1;
    }
    char *port_str = strchr(to[i].ip, ':');
    if (!port_str) {
      fprintf(stderr, "Invalid format in servers file\n");
      fclose(file);
      free(to);
      return 1;
    }
    *port_str++ = '\0'; // разделение ip-адреса и порта
    to[i].port = atoi(port_str);
  }
  fclose(file);

  // подключкние к каждому серверу и отправление заданий
  for (int i = 0; i < servers_num; i++) {
    struct hostent *hostname = gethostbyname(to[i].ip);
    if (hostname == NULL) {
      fprintf(stderr, "gethostbyname failed with %s\n", to[i].ip);
      exit(1);
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(to[i].port);
    server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);
    int sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck < 0) {
      fprintf(stderr, "Socket creation failed!\n");
      exit(1);
    }
    if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
      fprintf(stderr, "Connection failed\n");
      exit(1);
    }

    // само отправление
    uint64_t begin = 1;
    uint64_t end = k;
    char task[sizeof(uint64_t) * 3];
    memcpy(task, &begin, sizeof(uint64_t));
    memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
    memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));
    if (send(sck, task, sizeof(task), 0) < 0) {
      fprintf(stderr, "Send failed\n");
      exit(1);
    }

    // получение результатов
    char response[sizeof(uint64_t)];
    if (recv(sck, response, sizeof(response), 0) < 0) {
      fprintf(stderr, "Receive failed\n");
      exit(1);
    }
    uint64_t answer = 0;
    memcpy(&answer, response, sizeof(uint64_t));
    printf("answer: %llu\n", answer);

    close(sck);
  }
  free(to);
  return 0;
}

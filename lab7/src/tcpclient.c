#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

//#define BUFSIZE 100
#define SADDR struct sockaddr
//#define SIZE sizeof(struct sockaddr_in)

int main(int argc, char *argv[]) {

  if (argc != 4) {//проверка
    fprintf(stderr, "Usage: %s <ip> <port> <bufsize>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  const char *ip = argv[1];
  const int PORT = atoi(argv[2]);
  const size_t BUFSIZE = atoi(argv[3]);
  const size_t SIZE = sizeof(struct sockaddr_in);
  
  int fd;
  int nread;
  char buf[BUFSIZE];
  struct sockaddr_in servaddr;


  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creating");
    exit(1);
  }

  memset(&servaddr, 0, SIZE);
  servaddr.sin_family = AF_INET;

  if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
    perror("bad address");
    exit(1);
  }

  servaddr.sin_port = htons(atoi(argv[2]));

  if (connect(fd, (SADDR *)&servaddr, SIZE) < 0) {
    perror("connect");
    exit(1);
  }

  write(1, "Input message to send\n", 22);
  while ((nread = read(0, buf, BUFSIZE)) > 0) {
    if (write(fd, buf, nread) < 0) {
      perror("write");
      exit(1);
    }
  }

  close(fd);
  exit(0);
}

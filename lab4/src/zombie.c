#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t child_pid = fork();

    if (child_pid > 0) {
        // Родительский процесс
        sleep(10); // Задержка для демонстрации
        printf("Parent process exiting\n");
    } else if (child_pid == 0) {
        // Дочерний процесс
        printf("Child process exiting\n");
        exit(0);
    } else {
        // Ошибка при вызове fork()
        perror("fork");
        return 1;
    }

    return 0;
}

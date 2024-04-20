#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // Проверка количества аргументов
    if (argc != 3) {
        printf("Usage: %s <seed> <array_size>\n", argv[0]);
        return 1;
    }

    // Аргументы для передачи в sequential_min_max
    char *args[] = {"./sequential_min_max", argv[1], argv[2], NULL};

    // Вызов execvp для запуска sequential_min_max в отдельном процессе
    execvp(args[0], args);

    // Если execvp завершился неудачно, например из-за ошибки в имени программы
    perror("execvp");
    return 1;
}

#include "revert_string.h"
#include <stdlib.h>
#include <string.h>

void RevertString(char *str)
{
  // вычисляем длину строки
  int length = strlen(str);
  // выделяем память для временного хранения перевернутой строки
  char *temp = (char *)malloc((length + 1) * sizeof(char));
  // переворачиваем строку, копируя символы с конца в начало
  for (int i = 0; i < length; ++i)
  {
      temp[i] = str[length - i - 1];
  }
  // завершаем временную строку нулевым символом
  temp[length] = '\0';
  // копируем перевернутую строку обратно в исходную строку
  strcpy(str, temp);
  // освобождаем память, выделенную для временной строки
  free(temp);
}


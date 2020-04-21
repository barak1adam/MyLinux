/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
  char *feature_name;
  char str_arr[6][20] = {
    "XG-312-license aaa",
    "XG-312-license bbb",
    "XG-312-license ccc",
    "XG-312-license ddd",
    "XG-312-license eee",
    "XG-312-license fff",
  };

  for (int i =0; i<6; i++)
  {
    feature_name = (char *)(str_arr + i);
    printf("string in idx %d is: %s\n\r", i, feature_name);
  }

  printf("\n\n\n");

  char *str_ptr_arr[6] = {
    "XG-312-license aaa",
    "XG-312-license bbb",
    "XG-312-license ccc",
    "XG-312-license ddd",
    "XG-312-license eee",
    "XG-312-license fff",
  };

  for (int i =0; i<6; i++)
  {
    feature_name = str_ptr_arr[i];
    printf("string in idx %d is: %s\n\r", i, feature_name);
  }
  return 0;
}




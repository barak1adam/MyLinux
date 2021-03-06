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
  FILE *source, *target;
  char buf[1000];
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

  return 0;

  source = fopen("/tmp/src_file", "r");
  if (source == NULL){
    fclose(source);
    return 1;
  }

  target = fopen("/tmp/dst_file", "w");
  if (target == NULL)
  {
    fclose(target);
    return 1;
  }

  while (fgets(buf,1000, source)!=NULL)
	if (strstr(buf, "barak") == NULL)
		fputs(buf, target);

  fclose(source);
  fclose(target);

  /* rename is atomic */
  if (-1 == rename ("/tmp/dst_file", "/tmp/src_file")) {
    printf("%s: Rename failed\n\r", __FUNCTION__);
    unlink("/tmp/dst_file");
    return 1;
  }
  else
    unlink("/tmp/dst_file");

  return 0;
}




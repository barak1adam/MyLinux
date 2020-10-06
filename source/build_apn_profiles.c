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

#define APN_DEFAULT_PROFILES_FILE "./default_profiles"
#define APN_RUNNING_PROFILES_FILE "./running_profiles"

#define APN_DUPLICATED_ENTRY_ERROR 1
#define APN_MAX_PROFILE_ENTRIES 16

typedef struct {
  char             profile_name[64];
  int              priority;
  char             apn[64];
  char             ip_type[64];
  char             band[64];
} APN_PROFILE_ENTRY_T;

APN_PROFILE_ENTRY_T uid_input[APN_MAX_PROFILE_ENTRIES] = {
  {"mainland-new", 3, "israel.net.co.il", "ipv4v6", "09"},
  {"mainland-dup", 1, "ethernet.net.au", "ipv4v6", "09"},
  {"mainland", 3, "ethernet.net.au", "ipv4v6", "09"},
  {"mainland", 3, "ethernet.net.au", "ipv4v6", "09"},
  {"mainland", 2, "ethernet.net.au", "ipv4v6", "09"},
  {"barak", 2, "ethernet.net.au", "ipv4v6", "09"},
};

/* returns TRUE if profile name exists, FALSE otherwise */
static int find_apn_entry_by_profile_name(char *profile_name, char *file)
{
  FILE *fd;
  char buf[1000];
  APN_PROFILE_ENTRY_T curr_entry;

  fd = fopen(file, "r");
  if (fd == NULL){
    fclose(fd);
    return 0;
  }

  while (fgets(buf,1000, fd)!=NULL){
    sscanf(buf, "%s %d %s %s %s", curr_entry.profile_name, &curr_entry.priority, curr_entry.apn, curr_entry.ip_type, curr_entry.band);
    if (!strcmp(curr_entry.profile_name, profile_name)){
      fclose(fd);
      printf("profile name entry found !!!\n\r");
      return 1;
    }
  }

  fclose(fd);
  return 0;

}


/* returns TRUE if an entry found with the same apn tupple, FALSE otherwise */
static int find_duplicated_apn_entry(APN_PROFILE_ENTRY_T *entry, char *file)
{
  FILE *fd;
  char buf[1000];
  APN_PROFILE_ENTRY_T curr_entry;

  fd = fopen(file, "r");
  if (fd == NULL){
    fclose(fd);
    return 0;
  }

  while (fgets(buf,1000, fd)!=NULL){
    sscanf(buf, "%s %d %s %s %s", curr_entry.profile_name, &curr_entry.priority, curr_entry.apn, curr_entry.ip_type, curr_entry.band);
    if (!strcmp(curr_entry.apn, entry->apn) && !strcmp(curr_entry.ip_type, entry->ip_type) && !strcmp(curr_entry.band, entry->band) ){
      fclose(fd);
      printf("duplicated entry !!!! \n\r");
      return 1;
    }
  }

  fclose(fd);
  return 0;

}

static void copy_file(FILE *src_file, FILE *dest_file){

  int ch;

  if ((!src_file) || (!dest_file))
    return;

  while ((ch = fgetc(src_file)) != EOF)
	fputc(ch, dest_file);

  return;

}

static int add_apn_entry(APN_PROFILE_ENTRY_T *entry, char *file)
{
  FILE *fd = NULL;
  char buf[300] = {0};

  fd = fopen(file, "r+");
  if (fd == NULL){
    fclose(fd);
    return 1;
  }

  /* go to the end of the file*/
  if(fseek(fd, 0, SEEK_END) < 0)
    return 1;

  sprintf(buf, "%s %d %s %s %s\n\r", entry->profile_name, entry->priority, entry->apn, entry->ip_type, entry->band);
  if (fputs (buf, fd) == EOF){
    fclose(fd);
    printf("entry not added !!! \n\r");
    return 1;
  }

  printf("entry added \n\r");
  fclose(fd);
  return 0;

}

static int delete_apn_entry(char *profile_name, char *path_to_file)
{
  char cmd[100] = {0};

  /* remove apn entry from the file */
  sprintf(cmd, "sed -i '/%s /d' %s",profile_name, path_to_file);
  return system(cmd);

}


int main (int argc, char *argv[])
{
  FILE *source, *target;
  char buf[1000];
  char *profile_name;
  APN_PROFILE_ENTRY_T curr_entry;

  /* Creates an empty file for writing. 
   * If a file with the same name already exists, its content is erased and the file is considered as a new empty file. */
  target = fopen(APN_RUNNING_PROFILES_FILE, "w");
  if (target == NULL)
  {
    printf("file running_profiles could not created ! \n\r");
    return 1;
  }

  source = fopen(APN_DEFAULT_PROFILES_FILE, "r");
  if (source == NULL){
    fclose(source);
    return 1;
  }

  copy_file(source, target);

  fclose(source);
  fclose(target);

  /* for each new & legal entry recieved from CLI config */
  for (int i =0; i<APN_MAX_PROFILE_ENTRIES; i++)
  {

    if (strlen(uid_input[i].profile_name)){
      printf("APN entry in idx %d is: %s %d %s %s %s\n\r", i, uid_input[i].profile_name, uid_input[i].priority, uid_input[i].apn, uid_input[i].ip_type, uid_input[i].band);
      /* if identifier found in the table - overwrite the existing one */
      if ( find_apn_entry_by_profile_name(uid_input[i].profile_name, APN_RUNNING_PROFILES_FILE) )
	delete_apn_entry(uid_input[i].profile_name, APN_RUNNING_PROFILES_FILE);
      /* else - identifier not found */
      else{
	/* if its APN tuple is found in the table - return a duplicated error */
	if (find_duplicated_apn_entry(&uid_input[i], APN_RUNNING_PROFILES_FILE))
	    //return APN_DUPLICATED_ENTRY_ERROR;
	    continue;
      }
      add_apn_entry(&uid_input[i], APN_RUNNING_PROFILES_FILE);
    }
  }

  return 0;
}




#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void set_new_path(char* start_path, char* file_name, char* new_path)
{
  memcpy(new_path, start_path, strlen(start_path));
  new_path[strlen(start_path)] = '/';
  memcpy(new_path + strlen(start_path) + 1, file_name, strlen(file_name));
  new_path[strlen(start_path) + strlen(file_name) + 1] = '\x00';
}

int good_file_name(char* path, char* file_name)
{
  char* last_slash = path + strlen(path);
  while (*last_slash != '/' && last_slash >= path)
  {
    last_slash--;
  }
  return strcmp(file_name, last_slash + 1) == 0;
}

void find(char* path, char* file_name)
{
  char buf[512];
  int fd;
  struct dirent de;
  struct stat st;

  if (stat(path, &st) < 0)
  {
    fprintf(2, "find: cannot stat %s\n", path);
    return;
  }

  if (st.type == T_FILE && good_file_name(path, file_name))
  {
    printf("%s\n", path);
  }
  else if (st.type == T_DIR)
  {
    if ((fd = open(path, 0)) < 0)
    {
      fprintf(2, "find: cannot open %s\n", path);
      return;
    }

    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
      if (de.inum == 0)
        continue;
      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      set_new_path(path, de.name, buf);
      find(buf, file_name);
    }

    close(fd);
  }
}

int main(int argc, char* argv[])
{
  if (argc != 3)
  {
    fprintf(1, "wrong amount of arguments");
    exit(0);
  }
  find(argv[1], argv[2]);
  exit(0);
}

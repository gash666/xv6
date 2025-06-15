#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void xargs(int argc, char* argv[])
{
  char buf[512];
  char* args[argc + 1];
  args[argc] = '\x00';
  args[argc - 1] = buf;

  memcpy(args, argv + 1, (argc - 1) * sizeof(char*));

  char* pointer = buf;
  char now;
  while (read(0, &now, 1) == 1)
  {
    *pointer = now;
    if (now == '\n')
    {
      *pointer = '\x00';
      int pid = fork();
      if (pid == 0)
      {
        exec(args[0], args);
      }
      else
        pointer = buf - 1;
    }
    pointer++;
  }
}

int main(int argc, char* argv[])
{
  xargs(argc, argv);
  exit(0);
}

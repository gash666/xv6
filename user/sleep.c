#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int sleep2(int ticks)
{
  return sleep(ticks);
}

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    fprintf(1, "wrong amount of arguments");
    exit(0);
  }
  sleep(atoi(argv[1]));
  exit(0);
}

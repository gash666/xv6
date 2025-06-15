#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void pingpong()
{
  int pipe1[2], pipe2[2];
  pipe(pipe1);
  pipe(pipe2);
  int val = fork();
  if (val == 0)
  {
    char buf[1];
    read(pipe1[0], buf, 1);
    close(pipe1[0]);
    close(pipe1[1]);
    int id = getpid();
    printf("%d: received ping\n", id);
    write(pipe2[1], buf, 1);
    close(pipe2[0]);
    close(pipe2[1]);
    exit(0);
  }
  else
  {
    write(pipe1[1], "\x00", 1);
    close(pipe1[0]);
    close(pipe1[1]);
    wait(0);
    char buf[1];
    read(pipe2[0], buf, 1);
    close(pipe2[0]);
    close(pipe2[1]);
    int id = getpid();
    printf("%d: received pong\n", id);
    exit(0);
  }
}

int main(int argc, char* argv[])
{
  if (argc != 1)
  {
    fprintf(1, "wrong amount of arguments");
    exit(0);
  }
  pingpong();
  exit(0);
}

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int read_from_pipe(int buf[], int fd)
{
  int temp, bytes_read = 0;
  while (1)
  {
    temp = read(fd, ((char*)buf) + bytes_read, 4);
    if (temp == 0)
      break;
    bytes_read += temp;
  }
  return bytes_read / 4;
}

int primes(int rec, int fd)
{
  if (rec == 6)
  {
    int buf[35];
    int number = read_from_pipe(buf, fd);
    for (int i = 0; i < number; i++)
      printf("prime %d\n", buf[i]);
    close(fd);
    exit(0);
  }

  int pipe1[2];
  pipe(pipe1);
  int pid = fork();
  if (pid == 0)
  {
    close(pipe1[1]);
    primes(rec + 1, pipe1[0]);
    exit(0);
  }
  else
  {
    if (fd == -1)
    {
      int buf[35];
      for (int i = 1; i <= 35; i++)
        buf[i - 1] = i;
      write(pipe1[1], buf, 35 * 4);
    }
    else
    {
      int buf[35];
      int number = read_from_pipe(buf, fd);
      for (int i = 0; i < number; i++)
        if (buf[i] != 1 && (buf[i] % rec != 0 || rec >= buf[i]))
          write(pipe1[1], ((char*)buf) + i * 4, 4);
    }
    close(pipe1[1]);
    close(pipe1[0]);
    wait(&pid);
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
  primes(1, -1);
  exit(0);
}

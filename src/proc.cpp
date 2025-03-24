#include "proc.hpp"
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <raylib.h>

#ifdef __linux__
#include <unistd.h>
#include <sys/wait.h>

void Proc::launch(const char* ex, char* const* argv)
{
  child = fork();
  if (child == 0)
  {
    execvp(ex, argv);
    TraceLog(
      LOG_ERROR,
      "Subprocess failed with exit status %i: %s",
      (int)(errno),
      strerror(errno)
    );
    exit(1);
  }
  return;
}

void Proc::kill()
{
  ::kill(child, SIGKILL);
}

bool Proc::ended()
{
  static int zero_count = 0;
  // let the child die when it wants to
  int status;
  waitpid(child, &status, WNOHANG);
  if (status == 0)
  {
    zero_count++;
  }
  // me when I tried to kill a child
  // with nothing, because I was riled (sig 0)
  // just to make sure it stays aware
  // in the dark the demons do stare:
  return ::kill(child, 0) == ESRCH && zero_count > 10;
}

#endif /* __linux__ */


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
  // let the child die when it wants to
  pid_t rval = waitpid(child, &status, WNOHANG);
  if (rval == -1)
  {
    perror("waitpid failed");
    exit(1);
  }
  return rval != 0;
}

#elif defined(_WIN32)

void Proc::launch()

#else

#error Unknown target (Only __linux__ or _WIN32 are supported).

#endif /* __linux__ */


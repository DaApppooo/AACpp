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

void Proc::launch(const char* ex, char* const* argv)
{
  assert(argv == nullptr);
  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );
  if( !CreateProcess( NULL,   // No module name (use command line)
      (char*)ex,        // Command line
      NULL,           // Process handle not inheritable
      NULL,           // Thread handle not inheritable
      FALSE,          // Set handle inheritance to FALSE
      0,              // No creation flags
      NULL,           // Use parent's environment block
      NULL,           // Use parent's starting directory 
      &si,            // Pointer to STARTUPINFO structure
      &pi )           // Pointer to PROCESS_INFORMATION structure
  ) 
  {
    printf( "CreateProcess failed (%d).\n", GetLastError() );
    return;
  }
}

void Proc::kill()
{
  TerminateProcess(
    pi.hProcess,
    1U
  );
  CloseHandle( pi.hProcess );
  CloseHandle( pi.hThread );
}

bool Proc::ended()
{
  // Wait until child process exits.
  int status = WaitForSingleObject( pi.hProcess, 10 );
  if (status == WAIT_TIMEOUT)
    return false;
  else
  {
    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
    return true;
  }
  return false;
}

#else

#error Unknown target (Only __linux__ or _WIN32 are supported).

#endif /* __linux__ */


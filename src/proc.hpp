#ifndef H_PROC
#define H_PROC

#include <pthread.h>
#ifdef __linux__
#include <sys/types.h>
#endif

// A child process creator and manager
struct Proc
{
  int status;
  // search for ex in PATH, pass argv as arguments
  //
  // good old execvp style
  void launch(const char* ex, char* const* argv);
  // returns true if the child process has exited, false otherwise
  bool ended();
  // kill the child process
  void kill();

#ifdef __linux__
  pid_t child;
#elif defined(_WIN32)
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
#else
#error Unknown target.
#endif
};

#endif

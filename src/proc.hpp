#ifndef H_PROC
#define H_PROC

#include <pthread.h>
#ifdef __linux__
#include <sys/types.h>
#endif

// A child process creator and manager
struct Proc
{
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
#endif
};

struct Thread
{
  // Launch just a function
  void launch(void*(*f)(void*), void* data);
  // attach the process back to the parent (call this in the parent)
  void* join();
#ifdef __linux__
  pthread_t child;
#endif
};


#endif

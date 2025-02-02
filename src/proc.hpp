#ifndef H_PROC
#define H_PROC

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
  // Launch just a function
  void launch(void(*f)());
  // returns true if the child process has exited, false otherwise
  bool ended();
  // attach the process back to the parent (call this in the parent)
  void join();
  // kill the child process
  void kill();

#ifdef __linux__
  pid_t child;
#endif
};


#endif

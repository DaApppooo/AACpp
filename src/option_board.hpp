#ifndef H_OPTION_BOARD
#define H_OPTION_BOARD
#include "settings.hpp"

namespace current_board
{
  extern char path[1024];
  void init(Option& self);
  void mainbox(Option& self, Rectangle box);
  void set_path(const char* path);
}
namespace default_board
{
  extern char path[1024];
  void init(Option& self);
  void mainbox(Option& self, Rectangle box);
  void set_path(const char* path);
}
#endif

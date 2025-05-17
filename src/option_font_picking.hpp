#include "settings.hpp"

namespace font_picking
{
  extern char path[1024];
  void init(Option&);
  void destroy(Option&);
  void mainbox(Option& self, Rectangle box);
  Rectangle popup(Option& self);
  void set_path(const char* path);
}


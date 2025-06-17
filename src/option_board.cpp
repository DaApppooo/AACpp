#include "option_board.hpp"
#include "theme.hpp"

namespace current_board
{
  char path[1024];
  void set_path(const char* p)
  { strncpy(path, p, 1024); }
  void init(Option& self)
  {
    settings_try_pick_and_load_board();
  }
  void mainbox(Option& self, Rectangle box)
  {
    DrawTextEx(
      theme::fonts[0],
      "Load new board",
      {box.x+theme::gpad, box.y+box.height/2.f-theme::font_size},
      theme::font_size, theme::TEXT_SPACING,
      ~theme::text_color
    );
    draw_text_anchored(
      theme::fonts[0],
      path,
      {1.f, 1.f},
      {box.x+box.width, box.y+box.height},
      theme::font_size/1.5f,
      theme::TEXT_SPACING,
      fade((~theme::text_color), 0.5f)
    );
  }
}
namespace default_board
{
  char path[1024];
  void set_path(const char* p)
  { strncpy(path, p, 1024); }
  void init(Option& self)
  {
    set_path(current_board::path);
  }
  void mainbox(Option& self, Rectangle box)
  {
    DrawTextEx(
      theme::fonts[0],
      "Set default board to be the current one.",
      {box.x+theme::gpad, box.y+box.height/2.f-theme::font_size},
      theme::font_size, theme::TEXT_SPACING,
      ~theme::text_color
    );
    draw_text_anchored(
      theme::fonts[0],
      path,
      {1.f, 1.f},
      {box.x+box.width, box.y+box.height},
      theme::font_size/1.5f,
      theme::TEXT_SPACING,
      fade((~theme::text_color), 0.5f)
    );
  }
}

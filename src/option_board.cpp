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
      theme::font_size/2.f,
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
    PickAndLoadResult res = settings_try_once_pick_board(false);
    const char* mod = nullptr;
    switch (res.error)
    {
      case PickAndLoadResult::COMPILATION_ERROR:
        TraceLog(LOG_ERROR, "Failed to compile board (should not happen in this state).");
        break;
      case PickAndLoadResult::CANNOT_OPEN_FILE:
        TraceLog(LOG_ERROR, "Cannot open file.");
        break;
      case PickAndLoadResult::WRONG_FILE_FORMAT:
        TraceLog(LOG_ERROR, "Wrong file format. Expected '.obz' or '.cobz'.");
        break;
      case PickAndLoadResult::OK:
        mod = res.path;
        break;
    }
    if (mod)
      set_path(mod);
    current_options = -1;
  }
  void mainbox(Option& self, Rectangle box)
  {
    DrawTextEx(
      theme::fonts[0],
      "Change default board (loaded at startup)",
      {box.x+theme::gpad, box.y+box.height/2.f-theme::font_size},
      theme::font_size, theme::TEXT_SPACING,
      ~theme::text_color
    );
    draw_text_anchored(
      theme::fonts[0],
      path,
      {1.f, 1.f},
      {box.x+box.width, box.y+box.height},
      theme::font_size/2.f,
      theme::TEXT_SPACING,
      fade((~theme::text_color), 0.5f)
    );
  }
}

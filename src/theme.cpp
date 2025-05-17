#include "theme.hpp"
#include "clay.h"
#include "webcolor.hpp"
#include <cctype>
#include <cstdlib>
#include <raylib.h>

namespace theme
{
  const char* default_board = nullptr;
  Clay_Color background_color;
  Clay_Color text_color;
  Clay_Color text_field_bg_color;
  Clay_Color command_background;
  uint16_t gpad;
  uint16_t font_size;
  float border_weight;
  float text_space_width;
  Font* fonts;
  void update_font(Font f)
  {
    if (fonts[0].texture.id != 0)
      UnloadFont(fonts[0]);
    if (f.texture.id == 0)
    {
      fonts[0].texture.id = 0;
      return;
    }
    fonts[0] = f;
    text_space_width = MeasureTextEx(
      fonts[0],
      " ",
      font_size,
      TEXT_SPACING
    ).x;
  }
}


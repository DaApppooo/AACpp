#include "theme.hpp"
#include "clay.h"
#include "ini.h"
#include "webcolor.hpp"
#include <cctype>
#include <cstdlib>
#include <raylib.h>

Font font_array[1];
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
  Font* fonts = font_array;
}

#define LDPWARN(sec, prop) \
p = ini_get(cf, sec, prop); \
if (p == nullptr) { \
  TraceLog(LOG_WARNING, "Missing parameter in config: [%s]%s", sec, prop); \
} \
else


inline Clay_Color operator ~(Color rlcolor)
{
  return {
    (float)rlcolor.r,
    (float)rlcolor.g,
    (float)rlcolor.b,
    (float)rlcolor.a
  };
}

inline int int_parse_or(const char* s, int fallback)
{
  const char* end = s;
  int r = strtol(s, (char**)&end, 10);
  if (s == end)
    return fallback;
  return r;
}

void theme_load()
{
  // char* buffer = (char*)TextFormat("");
  const char* p = nullptr;
  ini_t* cf = ini_load("config/defaults.ini");
  LDPWARN("global", "background")
    theme::background_color = ~ color_parse_or(p, {53, 53, 53, 255});
  LDPWARN("global", "text_field_background")
    theme::text_field_bg_color = ~ color_parse_or(p, {117, 117, 117, 255});
  LDPWARN("global", "padding")
    theme::gpad = int_parse_or(p, 8);
  LDPWARN("global", "font_size")
    theme::font_size = int_parse_or(p, 8);
  LDPWARN("cell", "border_weight")
    theme::border_weight = int_parse_or(p, 2);
  LDPWARN("cell", "text")
    theme::text_color = ~ color_parse_or(p, WHITE);
  LDPWARN("commands", "background")
    theme::command_background = ~ color_parse_or(p, {100, 110, 255, 255});
  LDPWARN("global", "font_face")
  {
    if (isspace(*p) || *p == 0)
      font_array[0] = LoadFont("res/font.ttf");
    else
    {
      font_array[0] = LoadFont(p);
      if (font_array[0].texture.id == 0)
      {
        TraceLog(LOG_INFO, "Loading packaged font instead of specified font because of a failure.");
        font_array[0] = LoadFont("res/font.ttf");
      }
    }
  }
  ini_free(cf);
}


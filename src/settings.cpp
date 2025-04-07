#include "resman.hpp"
#include <algorithm>
extern "C"
{
#include "../include/iniparser.h"
}
#include "webcolor.hpp"
#include "theme.hpp"
#include "tts.hpp"
#define NO_CLAY
#include "clay.h"
#include "rlclay.h"

Font font_array[1];

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

#define LDPWARN(secprop) \
p = iniparser_getstring(cf, secprop, nullptr); \
if (p == nullptr) { \
  TraceLog(LOG_WARNING, "Missing parameter in config: %s", secprop); \
} \
else

void settings_load()
{
  const char* p = nullptr;
  dictionary* cf = iniparser_load("config/defaults.ini");
  if (cf == nullptr)
  {
    TraceLog(LOG_ERROR, "Failed to load settings file ! Program might look weird");
    return;
  }
  theme::fonts = font_array;
  LDPWARN("global:background")
    theme::background_color = ~ color_parse_or(p, {53, 53, 53, 255});
  LDPWARN("global:text_field_background")
    theme::text_field_bg_color = ~ color_parse_or(p, {117, 117, 117, 255});
  LDPWARN("global:padding")
    theme::gpad = int_parse_or(p, 8);
  LDPWARN("global:font_size")
    theme::font_size = int_parse_or(p, 8);
  LDPWARN("cell:border_weight")
    theme::border_weight = int_parse_or(p, 2);
  LDPWARN("cell:text")
    theme::text_color = ~ color_parse_or(p, WHITE);
  LDPWARN("commands:background")
    theme::command_background = ~ color_parse_or(p, {100, 110, 255, 255});
  LDPWARN("global:font_face")
  {
    if (isspace(*p) || *p == 0)
      font_array[0] = LoadFontEx("res/font.ttf", theme::font_size, 0, 0);
    else
    {
      font_array[0] = LoadFontEx(p, theme::font_size, 0, 0);
      if (font_array[0].texture.id == 0)
      {
        TraceLog(LOG_INFO, "Loading packaged font instead of specified font because of a failure.");
        font_array[0] = LoadFontEx("res/font.ttf", theme::font_size, 0, 0);
        assert(font_array[0].texture.id != 0);
      }
    }
  }
  tts_mode = TTS_PIPER; // default
  LDPWARN("tts:mode")
  {
    if (str_eq(p, "piper"))
      tts_mode = TTS_PIPER;
    else
      TraceLog(LOG_WARNING, "Unknown TTS client: %s", p);
  }
  LDPWARN("tts:param")
  {
    const int l = str_len(p);
    if (l > 1023)
      TraceLog(LOG_WARNING, "TTS parameters exceed allowed buffer (1023 characters).");
    memcpy(tts_param, p, std::min(l, 1024));
  }
  iniparser_freedict(cf);
}

void settings_save()
{
  
}

bool settings_open;

constexpr inline float max(float a, float b)
{
  // being explicit for g++
  if (a > b) return a;
  else return b;
}

void settings_update(Clay_RenderCommandArray& render_cmds)
{
  if (!settings_open)
    return;
  constexpr float MAX_WIDTH = 800.f;
  const float W = max(XMAX, MAX_WIDTH);
  const Rectangle rect = {
    XMAX/2.f - W/2.f, 0.f,
    W, YMAX
  };
  layout_options(render_cmds);
  BeginDrawing();
    Clay_Raylib_Render(render_cmds, theme::fonts);
  EndDrawing();
}

void destroy_settings()
{
}


#include "settings.hpp"
#include "proc.hpp"
#include "raylib.h"
#include "resman.hpp"
#include "utils.hpp"
#include <algorithm>
extern "C"
{
#include "../include/iniparser.h"
}
#include "webcolor.hpp"
#include "globals.hpp"
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

const char* settings_load()
{
  const char* p = nullptr;
  dictionary* cf = iniparser_load("assets/settings.ini");
  if (cf == nullptr)
  {
    const char* E = "Failed to load settings file !";
    TraceLog(LOG_ERROR, E);
    return E;
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
      font_array[0] = LoadFontEx("assets/font.ttf", theme::font_size, 0, 0);
    else
    {
      font_array[0] = LoadFontEx(p, theme::font_size, 0, 0);
      if (font_array[0].texture.id == 0)
      {
        TraceLog(LOG_INFO, "Loading packaged font instead of specified font because of a failure.");
        font_array[0] = LoadFontEx("assets/font.ttf", theme::font_size, 0, 0);
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
  { // default board
    p = iniparser_getstring(cf, "global:default_board", nullptr);
    if (p)
      current_board_file = {fopen(p, "rb")};
    if (p == nullptr || current_board_file._f == nullptr)
    {
      PickAndLoadResult res;
      res = settings_pick_and_load_board();
      while (res.error != PickAndLoadResult::OK && !WindowShouldClose())
      {
        while (!WindowShouldClose())
        {
          if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            break;
          BeginDrawing();
            ClearBackground(
              CLAY_COLOR_TO_RAYLIB_COLOR(theme::background_color)
            );
            const char* msg;
            switch (res.error)
            {
              case PickAndLoadResult::OK:
                break;
              case PickAndLoadResult::CANNOT_OPEN_FILE:
                msg = "Could not open file.";
                break;
              case PickAndLoadResult::WRONG_FILE_FORMAT:
        msg = "Wrong file format. '.obz' or '.cobz' file format is needed.";
                break;
              case PickAndLoadResult::COMPILATION_ERROR:
                msg = "Could not compile file. Check logs for more info.";
                break;
            }
            DrawTextEx(
              font_array[0],
              msg,
              {0,0}, theme::font_size, theme::text_space_width,
              CLAY_COLOR_TO_RAYLIB_COLOR(theme::text_color)
            );
            DrawTextEx(
              font_array[0],
              "Touch the screen to try again.",
              {0,float(theme::font_size)},
              theme::font_size, theme::text_space_width,
              CLAY_COLOR_TO_RAYLIB_COLOR(theme::text_color)
            );
          EndDrawing();
        }
        res = settings_pick_and_load_board();
      }
      current_board_file = res.f;
      if (WindowShouldClose())
        return "Did not open any file.";
    }
  }

  iniparser_freedict(cf);
  return nullptr;
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

inline float throbber_func(float x)
{
  static constexpr float speed = 4.f;
  return (sinf(speed*x)+1.f)/speed + x;
}
PickAndLoadResult settings_pick_and_load_board()
{
  const char* temp;
  char* child_param_buffer[4];
  Stream board_src;
  Proc child;

  extern list<Board> boards;
  boards.clear();
  alloc_board();

  const char* const unknown_board_path = open_file_dialogue();
  if (!(
     str_endswith(unknown_board_path, ".obz")
  || str_endswith(unknown_board_path, ".cobz")
  )) {
    return {PickAndLoadResult::WRONG_FILE_FORMAT, unknown_board_path, nullptr};
  }
  const char* compiled_board_path;
  if (IsFileExtension(unknown_board_path, ".cobz"))
  {
    compiled_board_path = unknown_board_path;
  }
  else if (
     FileExists(temp=TextFormat("%s.cobz", unknown_board_path))
  && FileEditTime(unknown_board_path) <= FileEditTime(temp)
  )
  {
    // file was already auto compiled and there's no new version
    compiled_board_path = temp;
  }
  else
  {
    child_param_buffer[0] = (char*)"assets/obz2cobz";
    child_param_buffer[1] = (char*)unknown_board_path;
    child_param_buffer[2] = (char*)TextFormat("%s.cobz", unknown_board_path);
    child_param_buffer[3] = nullptr;
    child.launch(
      "assets/obz2cobz",
      child_param_buffer
    );
    float t = 0;
    float dt;
    WaitTime(0.5);
    while (!child.ended())
    {
      if (WindowShouldClose())
      {
        child.kill();
        break;
      }
      XMAX = GetScreenWidth();
      YMAX = GetScreenHeight();
      BeginDrawing();
        ClearBackground(
          CLAY_COLOR_TO_RAYLIB_COLOR(theme::background_color)
        );
        const int w = MeasureText(
          "Compiling board... (done once per board set)",
          50
        );
        DrawText(
          "Compiling board... (done once per board set)",
          int(XMAX)/2 - w/2,
          YMAX/2 - 25,
          50,
          CLAY_COLOR_TO_RAYLIB_COLOR(theme::text_color)
        );
#define THROBBER(F) \
        DrawCircleV( \
          { \
            XMAX/2.f + cosf(throbber_func(t*(F))*2.f*PI)*20.f, \
            2.f*YMAX/3.f + sinf(throbber_func(t*(F))*2.f*PI)*20.f \
          }, \
          7.5f, \
          WHITE \
        )
      THROBBER(1);
      THROBBER(0.8);
      THROBBER(0.6);
      THROBBER(0.4);
      EndDrawing();
      dt = GetFrameTime();
      t += dt;
    }
    // NOTE: we can't use 'temp' because we can't be 100% sure it was set
    //  because of possible compiler shenanigans.
    compiled_board_path = TextFormat("%s.cobz", unknown_board_path);
  }

  board_src = {fopen(compiled_board_path, "rb")};
  if (board_src._f == nullptr)
  {
    TraceLog(
      LOG_FATAL,
      "Could not open compiled open board file '%s'.",
      compiled_board_path
    );
    return {
      PickAndLoadResult::CANNOT_OPEN_FILE,
      compiled_board_path, board_src
    };
  }
  return {PickAndLoadResult::OK, compiled_board_path, board_src};
}

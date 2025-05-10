#include "settings.hpp"
#include "proc.hpp"
#include "raylib.h"
#include "resman.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstring>
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

/*
  I use "option" to talk about individual parameters, while I use "settings" to
  talk about the menu.
*/
const char* DEFAULT_FONT = "assets/ShantellSans.ttf";
Font font_array[1];
struct Option;
enum OptionId
{
  OPT_CURRENT_BOARD,
  OPT_DEFAULT_BOARD,
  OPT_FONT,
  OPT_COUNT,
  OPT_FONT_SIZE,
  OPT_BG_COLOR,
  OPT_TEXT_COLOR,
  OPT_TEXT_FIELD_COLOR,
  OPT_COMMAND_BG_COLOR,
  OPT_PADDING,
  OPT_BORDER_WEIGHT,
};
bool settings_open;
int current_options;
fvec2 popup_position;
float settings_scroll;
using OptionStateFunc = void(*)(Option&);
struct Option
{
  OptionStateFunc init = nullptr; // called when box is clicked
  OptionStateFunc destroy = nullptr; // when popup is exited
  // main box in the settings menu
  void(*mainbox)(Option& self, Rectangle rec) = nullptr;
  // popup update (can use 'popup_position')
  // returns draw rectangle of popup
  Rectangle(*popup)(Option& self) = nullptr; 
  union {
    char path[1024];
    Clay_Color* color;
    uint16_t* size;
    float* width;
    float* weight;
  };
};
Rectangle find_valid_rect(float w, float h)
{
  Rectangle out = {popup_position.x, popup_position.y, w, h};
  if (out.x+w > XMAX)
    out.x -= w;
  if (out.y+h > YMAX)
    out.y -= h;
  return out;
}
namespace current_board
{
  void init(Option& self)
  {
    settings_try_pick_and_load_board();
  }
  void mainbox(Option& self, Rectangle box)
  {
    DrawTextEx(
      font_array[0],
      "Load new board",
      {box.x+theme::gpad, box.y+box.height/2.f-theme::font_size},
      theme::font_size, theme::TEXT_SPACING,
      ~theme::text_color
    );
    draw_text_anchored(
      font_array[0],
      self.path,
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
      strncpy(self.path, mod, 1024);
    current_options = -1;
  }
  void mainbox(Option& self, Rectangle box)
  {
    DrawTextEx(
      font_array[0],
      "Change default board (loaded at startup)",
      {box.x+theme::gpad, box.y+box.height/2.f-theme::font_size},
      theme::font_size, theme::TEXT_SPACING,
      ~theme::text_color
    );
    draw_text_anchored(
      font_array[0],
      self.path,
      {1.f, 1.f},
      {box.x+box.width, box.y+box.height},
      theme::font_size/2.f,
      theme::TEXT_SPACING,
      fade((~theme::text_color), 0.5f)
    );
  }
}
namespace font_picking
{
  list<char*> available_font_paths;
  list<Font> available_fonts;
  float scroll;
  void init(Option&)
  {
    scroll = 0.f;
    char* search_dirs;
  #ifdef __linux__
    char* xdg_data_dirs = getenv("XDG_DATA_DIRS");
    search_dirs = xdg_data_dirs
                      ? xdg_data_dirs
                      : (char*)TextFormat(
                       "/usr/share:/usr/local/share:%s/.local/share",
                       getenv("HOME"));
  #else
  # error init_font_picking for windows, android, ios: TODO
  #endif
    { // First load packaged font
      FilePathList fpl = LoadDirectoryFilesEx(
        "assets",
        "ttf", true
      );
      available_font_paths.extend({
        .start = fpl.paths,
        .len = fpl.count,
        .full = fpl.count
      });
      for (int i = 0; i < fpl.count; i++)
        fpl.paths[i] = 0; // disown strings
      UnloadDirectoryFiles(fpl);
    }

    // Then load system fonts
    char* start = search_dirs;
    char* end = search_dirs;
    available_font_paths.init();
    while (start != nullptr)
    {
      while (*end != ':' && *end != 0)
        end++;
      char* const next_start = *end ? end+1 : nullptr;
      *end = 0;
      FilePathList fpl = LoadDirectoryFilesEx(
        TextFormat("%s/fonts",start),
        "ttf", true
      );
      available_font_paths.extend({
        .start = fpl.paths,
        .len = fpl.count,
        .full = fpl.count
      });
      for (int i = 0; i < fpl.count; i++)
        fpl.paths[i] = 0; // disown strings
      UnloadDirectoryFiles(fpl);
      start = next_start;
    }
  }
  void destroy(Option&)
  {
    available_font_paths.destroy();
  }
  void mainbox(Option& self, Rectangle box)
  {
    DrawTextEx(
      font_array[0],
      "Current font family",
      {box.x+theme::gpad, box.y+box.height/2.f-theme::font_size},
      theme::font_size, theme::TEXT_SPACING,
      ~theme::text_color
    );
  }
  Rectangle popup(Option& self)
  {
    const Rectangle r = find_valid_rect(float w, float h)
  }
}

#define DECL_IM(module) {.init=module::init,.mainbox=module::mainbox}
#define DECL_IDM(module) {.init=module::init,.destroy=module::destroy,\
                    .mainbox=module::mainbox}
#define DECL_IDMP(module) {.init=module::init,.destroy=module::destroy,\
                    .mainbox=module::mainbox,.popup=module::popup}
Option options[OPT_COUNT] = {
  DECL_IM(current_board),
  DECL_IM(default_board),
  DECL_IDMP(font_picking)
};


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

void set_default_board(const char* path)
{
  strncpy(options[OPT_DEFAULT_BOARD].path, path, 1024);
}

const char* settings_load()
{
  const char* p = nullptr;
  font_picking::available_font_paths.init();
  dictionary* cf = iniparser_load("assets/settings.ini");
  if (cf == nullptr)
  {
    const char* E = "Failed to load settings file !";
    TraceLog(LOG_ERROR, E);
    return E;
  }
  theme::fonts = font_array;
  LDPWARN("global:background")
  {
    theme::background_color = ~ color_parse_or(p, {53, 53, 53, 255});
    // options[OPT_BG_COLOR].color = &theme::background_color;
  }
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
    const char* path = nullptr;
    if (isspace(*p) || *p == 0)
    {
      path = DEFAULT_FONT;
      font_array[0] = LoadFontEx(path, theme::font_size, 0, 0);
    }
    else
    {
      font_array[0] = LoadFontEx(p, theme::font_size, 0, 0);
      if (font_array[0].texture.id == 0)
      {
        TraceLog(LOG_INFO, "Loading packaged font instead of specified font because of a failure.");
        path = DEFAULT_FONT;
        font_array[0] = LoadFontEx(path, theme::font_size, 0, 0);
        assert(font_array[0].texture.id != 0);
      }
      else
        path = p;
    }
    strncpy(options[OPT_FONT].path, path, 1024);
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
      source_cobz = fopen(p, "rb");
    if (p == nullptr || source_cobz == nullptr)
    {
      settings_try_pick_and_load_board();
      set_default_board(options[OPT_CURRENT_BOARD].path);
    }
  }

  iniparser_freedict(cf);
  return nullptr;
}

void settings_save()
{
  dictionary* cf = iniparser_load("assets/settings.ini");

  if (cf == nullptr)
  {
    const char* E = "Failed to load settings file !";
    TraceLog(LOG_ERROR, E);
    return;
  }

  iniparser_set(cf, "global:default_board", options[OPT_DEFAULT_BOARD].path);
  // iniparser_set(cf, "global:background", fmt(*options[OPT_BG_COLOR].color));
  
  iniparser_set(cf, "global:font_face", options[OPT_FONT].path);

  FILE* f = fopen("assets/settings.ini", "w");
  iniparser_dump(cf, f);
  fclose(f);
  iniparser_freedict(cf);
}

constexpr inline float max(float a, float b)
{
  // being explicit for g++
  if (a > b) return a;
  else return b;
}

void settings_update(Clay_RenderCommandArray& render_cmds)
{
  constexpr float MAX_WIDTH = 800.f;
  const float OPT_H = 50.f + theme::font_size;
  const Rectangle back_btn_rec =
    {float(theme::gpad), 0.f, theme::BAR_HEIGHT, theme::BAR_HEIGHT};
  Rectangle popup_rect = {0.f, 0.f, 0.f, 0.f};
  BeginDrawing();
  {
    ClearBackground(~theme::background_color);
    for (int i = 0; i < OPT_COUNT; i++)
    {
      const Rectangle rec = {
        max(XMAX/2.f - MAX_WIDTH/2.f, 0.f),
        i*OPT_H - settings_scroll + theme::BAR_HEIGHT,
        MAX_WIDTH > XMAX ? XMAX : MAX_WIDTH,
        OPT_H
      };
      DrawLineEx(
        {rec.x,rec.y+rec.height},
        {rec.x+rec.width,rec.y+rec.height},
        theme::border_weight,
        fade((~theme::text_color), 0.5f)
      );
      // DrawRectangleLinesEx(rec, theme::border_weight, ~theme::text_color);
      if (options[i].mainbox)
        options[i].mainbox(options[i], rec);
      if (current_options == -1
          && ctrl::clicked()
          && CheckCollisionPointRec(ctrl::mpos, rec)
      ) {
        if (options[i].init)
        {
          current_options = i;
          options[i].init(options[i]);
        }
      }
    }
    { // top bar
      DrawRectangleRec({0.f, 0.f, XMAX, theme::BAR_HEIGHT},
                       ~theme::command_background);
      DrawTexturePro(
        btns[BTI_BACKSPACE],
        {0.f, 0.f, theme::IMG_SCALE.width, theme::IMG_SCALE.height},
        back_btn_rec,
        {0.f, 0.f},
        0.f,
        WHITE
      );
      if (CheckCollisionPointRec(ctrl::mpos, back_btn_rec) && ctrl::clicked())
      {
        settings_open = false;
        SetTargetFPS(BOARD_FPS);
        ctrl::clear_input();
      }
    }
    if (current_options != -1 && options[current_options].popup != nullptr)
    {
      popup_rect =
        options[current_options].popup(options[current_options]);
      if (ctrl::clicked()
          && !CheckCollisionPointRec(ctrl::mpos, popup_rect)
      ) {
        options[current_options].destroy(options[current_options]);
        current_options = -1;
      }
    }
  }
  EndDrawing();
  if (!hover(popup_rect))
  {
    settings_scroll += ctrl::delta_scroll();
    settings_scroll = Clamp(
      settings_scroll, 0.f, OPT_H*(float(OPT_COUNT)-1.f)
    );
  }
}

void destroy_settings()
{
}

inline float throbber_func(float x)
{
  static constexpr float speed = 4.f;
  return (sinf(speed*x)+1.f)/speed + x;
}

const char* settings_try_pick_and_load_board()
{
  PickAndLoadResult res;
  res = settings_try_once_pick_board(true);
  while (res.error != PickAndLoadResult::OK && !WindowShouldClose())
  {
    while (!WindowShouldClose())
    {
      if (ctrl::clicked())
        break;
      BeginDrawing();
        ClearBackground(~theme::background_color);
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
          ~theme::text_color
        );
        DrawTextEx(
          font_array[0],
          "Touch the screen to try again.",
          {0,float(theme::font_size)},
          theme::font_size, theme::text_space_width,
          ~theme::text_color
        );
      EndDrawing();
    }
    res = settings_try_once_pick_board(true);
  }
  source_cobz = res.f._f;
  reset_res();
  current_options = -1;
  if (res_load_boardset({source_cobz}))
  {
    return "Failed to load board.";
  }
  strncpy(options[OPT_CURRENT_BOARD].path, res.path, 1024);
  return nullptr;
}

PickAndLoadResult settings_try_once_pick_board(bool clear)
{
  const char* temp;
  char* child_param_buffer[4];
  Stream board_src;
  Proc child;

  extern list<Board> boards;

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
        ClearBackground(~theme::background_color);
        const int w = MeasureText(
          "Compiling board... (done once per board set)",
          50
        );
        DrawText(
          "Compiling board... (done once per board set)",
          int(XMAX)/2 - w/2,
          YMAX/2 - 25,
          50,
          ~theme::text_color
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

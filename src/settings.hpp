#pragma once
#include "globals.hpp"
#include "utils.hpp"
#ifndef H_SETTINGS
#define H_SETTINGS
#define NO_CLAY
#include "clay.h"

struct PickAndLoadResult
{
  enum Error
  {
    OK, WRONG_FILE_FORMAT, CANNOT_OPEN_FILE,
    COMPILATION_ERROR
  } error;
  const char* path;
  Stream f;
};
extern const char* DEFAULT_FONT;
const char* settings_load();
void settings_save();
const char* settings_try_pick_and_load_board();
PickAndLoadResult settings_try_once_pick_board(bool clear);

/* Options GUI. */
extern bool settings_open; // could be made float for animations

void init_settings();
void settings_update(Clay_RenderCommandArray& render_cmds); // includes drawing
void destroy_settings();

struct Option;
extern bool settings_open;
extern int current_options;
extern fvec2 popup_position;
extern float settings_scroll;
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
};
enum OptionId
{
  OPT_CURRENT_BOARD,
  OPT_DEFAULT_BOARD,
  OPT_FONT,
  OPT_FONT_SIZE,
  OPT_BORDER_WEIGHT,
  OPT_PADDING,
  OPT_BG_COLOR,
  OPT_TEXT_COLOR,
  OPT_TEXT_FIELD_COLOR,
  OPT_COMMAND_BG_COLOR,
  OPT_COUNT
};
extern Option options[OPT_COUNT];
Rectangle find_valid_rect(float w, float h);

#endif /* H_SETTINGS */

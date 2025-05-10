#pragma once
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
extern float settings_scroll;
extern const char* DEFAULT_FONT;
const char* settings_load();
void settings_save();
const char* settings_try_pick_and_load_board();
PickAndLoadResult settings_try_once_pick_board(bool clear);
void set_default_board(const char* path);

/* Options GUI. */
extern bool settings_open; // could be made float for animations

void init_settings();
void settings_update(Clay_RenderCommandArray& render_cmds); // includes drawing
void destroy_settings();

#endif /* H_SETTINGS */

#pragma once
#ifndef H_SETTINGS
#define H_SETTINGS
#define NO_CLAY
#include "clay.h"

void settings_load();
void settings_save();


/* Options GUI. */
extern bool settings_open; // could be made float for animations
inline void settings_show()
{
  settings_open = true;
}
void init_settings();
void settings_update(Clay_RenderCommandArray& render_cmds); // includes drawing
void destroy_settings();


#endif /* H_SETTINGS */

#pragma once
#include "raylib.h"
#include "utils.hpp"
#ifndef H_GLOBALS
#define H_GLOBALS

using fvec2 = Vector2;

extern float XMAX, YMAX;
extern Stream current_board_file;
namespace ctrl
{
  extern fvec2 mpos;
  extern bool touch_press;
}

#endif

#pragma once
#include "raylib.h"
#ifndef H_GLOBALS
#define H_GLOBALS

using fvec2 = Vector2;

extern float XMAX, YMAX;
namespace ctrl
{
  extern fvec2 mpos;
  extern bool touch_press;
}

#endif

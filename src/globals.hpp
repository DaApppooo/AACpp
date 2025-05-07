#pragma once
#include "raylib.h"
#include "resman.hpp"
#include <cstdint>
#ifndef H_GLOBALS
#define H_GLOBALS

using fvec2 = Vector2;

extern float XMAX, YMAX;
namespace ctrl
{
  extern fvec2 mpos;
  extern bool touch_press;
}
inline Color operator ~ (Clay_Color c)
{ return {uint8_t(c.r),uint8_t(c.g),uint8_t(c.b),uint8_t(c.a)}; }

#endif

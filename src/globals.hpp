#pragma once
#include "raylib.h"
#include "resman.hpp"
#include <cstdint>
#define NO_CLAY
#include "clay.h"
#ifndef H_GLOBALS
#define H_GLOBALS

#define rgb(R,G,B) {uint8_t(R), uint8_t(G), uint8_t(B), 255}
#define fade(COLOR, F0_1) {COLOR.r, COLOR.g, COLOR.b, uint8_t(F0_1*255.f)}
#ifdef _WIN32
#define PATH_DELIM '\\'
#else
#define PATH_DELIM '/'
#endif

using fvec2 = Vector2;
constexpr int BOARD_FPS = 15; // nothing moves, so no need to have more
constexpr int SETTINGS_FPS = 30;

extern float XMAX, YMAX;
extern float dt;
namespace ctrl
{
  extern fvec2 mpos;
  extern fvec2 mdpos;
  bool clicked();
  void update();
  float delta_scroll();
  void clear_input();
}
inline const char* fmt(Clay_Color c)
{
  return TextFormat("rgb(%i,%i,%i)", (int)c.r, (int)c.g, (int)c.b);
}
inline bool hover(Rectangle rec)
{
  return CheckCollisionPointRec(ctrl::mpos, rec);
}
void draw_text_anchored(
  Font font,
  const char* text,
  fvec2 relative_percent_anchor,
  fvec2 absolute_anchor,
  float font_size,
  float spacing,
  Color color
);
inline Color operator ~ (Clay_Color c)
{ return {uint8_t(c.r),uint8_t(c.g),uint8_t(c.b),uint8_t(c.a)}; }

#endif

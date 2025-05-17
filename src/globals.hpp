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
  extern fvec2 click_pos;
  bool clicked();
  void update();
  float delta_scroll();
  void clear_input();
  float delta_scroll_in(Rectangle r);
  inline bool hover(Rectangle rec)
  {
    return CheckCollisionPointRec(ctrl::mpos, rec);
  }
}
inline fvec2 center(Rectangle r)
{ return {r.x+r.width/2.f, r.y+r.height/2.f}; }
inline const char* fmt(Clay_Color c)
{
  return TextFormat("rgb(%i,%i,%i)", (int)c.r, (int)c.g, (int)c.b);
}
const char* fmt(int x);
// removes both the extension and the path part
inline const char* filename(const char* s)
{
  int i = 0;
  int len_before_last_slash = 0;
  while (s[i] != '.' && s[i] != 0)
  {
    if (s[i] == '/')
      len_before_last_slash = 0;
    else
      len_before_last_slash++;
    i++;
  }
  char* out = (char*)TextFormat(" ");
  while (s[i] != '/' && i >= 0)
  {
    out[--len_before_last_slash] = s[--i];
  }
  return out;
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
inline Clay_Color operator ~ (Color c)
{ return {float(c.r),float(c.g),float(c.b),float(c.a)}; }

#endif

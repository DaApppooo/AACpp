#include "globals.hpp"
#include "raylib.h"
#include "raymath.h"
#include <cmath>
float XMAX, YMAX;
float dt;
namespace ctrl
{
  fvec2 mpos;
  fvec2 mdpos;
  fvec2 click_pos;
  float move_amount_since_clicked;
  float dead_click = 0.f;
  bool _clicked = false;
  bool _spring = true;
  bool clicked()
  {
    return _clicked;
  }
  void clear_input()
  {
    // dead_click = 0.1f;
  }
  float delta_scroll()
  {
    // if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    //   return -mdpos.y;
    // return 0.f;
    return GetMouseWheelMove() * -30;
  }
  float delta_scroll_in(Rectangle r)
  {
    if (CheckCollisionPointRec(click_pos, r))
    {
      return delta_scroll();
    }
    return 0.f;
  }
  void update()
  {
    mpos = GetMousePosition();
    dt = GetFrameTime();
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
      if (_spring)
      {
        _clicked = true;
        _spring = false;
      }
      else
        _clicked = false;
    }
    else
    {
      _spring = true;
      _clicked = false;
    }
    if (dead_click > 0.f)
      dead_click -= dt;
    // if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    // {
    //   move_amount_since_clicked = 0.f;
    //   click_pos = mpos;
    // }
    // if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    // {
    //   click_pos = {-1.f, -1.f};
    // }
    mdpos = GetMouseDelta();
    // move_amount_since_clicked += Vector2Length(mdpos);
  }
}

const char* fmt(int x)
{
  if (x == 0)
    return "0";
  char* s = (char*)TextFormat(" ");
  char* p = s;
  while (x > 0)
  {
    *(p++) = '0' + (x%10);
    x /= 10;
  }
  *p = 0;
  char* q = s;
  while (q < p)
  { // reverse the string
    char temp = *q;
    *q = *(--p);
    *p = temp;
    q++;
  }
  return s;
}

void draw_text_anchored(
  Font font,
  const char* text,
  fvec2 relative_percent_anchor,
  fvec2 absolute_anchor,
  float font_size,
  float spacing,
  Color color
)
{
  const fvec2 m = MeasureTextEx(font, text, font_size, spacing);
  DrawTextPro(
    font,
    text,
    absolute_anchor,
    {m.x*relative_percent_anchor.x,m.y*relative_percent_anchor.y},
    0.f,
    font_size, spacing, color
  );
}

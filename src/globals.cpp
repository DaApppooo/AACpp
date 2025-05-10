#include "globals.hpp"
#include "raylib.h"
#include "raymath.h"
float XMAX, YMAX;
float dt;
namespace ctrl
{
  fvec2 mpos;
  fvec2 mdpos;
  float move_amount_since_clicked;
  float dead_click = 0.f;
  bool clicked()
  {
    return IsMouseButtonReleased(MOUSE_LEFT_BUTTON)
       && move_amount_since_clicked < 5.f
       && dead_click <= 0.f;
  }
  void clear_input()
  {
    dead_click = 0.1f;
  }
  float delta_scroll()
  {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
      return -mdpos.y;
    return 0.f;
  }
  void update()
  {
    mpos = GetMousePosition();
    dt = GetFrameTime();
    if (dead_click > 0.f)
      dead_click -= dt;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
      move_amount_since_clicked = 0.f;
    }
    mdpos = GetMouseDelta();
    move_amount_since_clicked += Vector2Length(mdpos);
  }
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

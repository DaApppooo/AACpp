
#include "option_colors.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "settings.hpp"
#include "theme.hpp"
#include "rlgl.h"
#include <cmath>
#include "raymath.h"

constexpr float COLOR_MENU_W = 350.f;
constexpr float COLOR_MENU_H = 350.f;
constexpr int COLOR_MENU_SEG = 48;
constexpr float COLOR_MENU_KNOB_RADIUS = 10.f;

void color_menu(Rectangle r, Vector3& triple)
{
  DrawRectangleRounded(
    r,
    0.1f,
    16,
    ~theme::background_color
  );
  DrawRectangleRoundedLinesEx(r, 0.1f, 16, theme::border_weight, ~theme::text_color);
  const Color current = ColorFromHSV(triple.x*RAD2DEG, triple.y, triple.z);
  const float radius =
    (r.width-theme::gpad*4.f-COLOR_MENU_KNOB_RADIUS*2.f)/2.f;
  const fvec2 wc = {
    r.x+radius+theme::gpad,
    r.y+r.height/2.f
  };
  const Color center = ColorFromHSV(0.f, 0.f, triple.z);
  Color prev = {255,0,0,255};
  rlBegin(RL_TRIANGLES);
      rlNormal3f(0.0f, 0.0f, 1.0f);
  for (int i = 0; i < COLOR_MENU_SEG; i++)
  {
    const float prev_angle = (i-1)*PI*2.f/COLOR_MENU_SEG;
    const float angle = i*PI*2.f/COLOR_MENU_SEG;
    const Color next = ColorFromHSV(angle*RAD2DEG, 1.f, 1.f);
        rlColor4ub(prev.r, prev.g, prev.b, prev.a);
        rlVertex2f(wc.x+cosf(prev_angle)*radius, wc.y+sinf(prev_angle)*radius);
        rlColor4ub(center.r, center.g, center.b, center.a);
        rlVertex2f(wc.x, wc.y);
        rlColor4ub(next.r, next.g, next.b, next.a);
        rlVertex2f(wc.x+cosf(angle)*radius, wc.y+sinf(angle)*radius);
    prev = next;
  }
  rlEnd();
  const fvec2 current_pos = {
    wc.x + cosf(triple.x)*triple.y*radius,
    wc.y + sinf(triple.x)*triple.y*radius
  };
  DrawCircleV(
    current_pos,
    COLOR_MENU_KNOB_RADIUS,
    current
  );
  DrawCircleLinesV(
    current_pos,
    COLOR_MENU_KNOB_RADIUS,
    ~theme::text_color
  );
  const float value_x = r.x + r.width - theme::gpad - COLOR_MENU_KNOB_RADIUS;
  const Rectangle value_rect = {
    value_x, r.y + float(theme::gpad),
    COLOR_MENU_KNOB_RADIUS, r.height-theme::gpad*2.f
  };
  DrawRectangleGradientV(
    value_rect.x, value_rect.y, value_rect.width, value_rect.height,
    BLACK, WHITE
  );
  const float value_current_y = value_rect.y + value_rect.height*triple.z;
  const fvec2 value_current_pos =
    {value_x + COLOR_MENU_KNOB_RADIUS/2.f, value_current_y};
  DrawCircleV(
    value_current_pos,
    COLOR_MENU_KNOB_RADIUS,
    ColorBrightness(WHITE, -1.f + triple.z)
  );
  DrawCircleLinesV(
    value_current_pos,
    COLOR_MENU_KNOB_RADIUS,
    ~theme::text_color
  );
  if (CheckCollisionPointCircle(ctrl::click_pos, wc, radius))
  {
    const fvec2 diff = Vector2ClampValue(ctrl::mpos - wc, 0.f, radius);
    triple.x = atan2f(diff.y, diff.x);
    triple.y = Vector2Length(diff)/radius;
  }
  if (CheckCollisionPointRec(ctrl::click_pos, value_rect))
  {
    const float diff = Clamp(
      ctrl::mpos.y - value_rect.y, 0.f, value_rect.height
    );
    triple.z = diff / value_rect.height;
  }
}

namespace background_color
{
  Vector3 triple;
  Rectangle popup_rect;
  void init(Option& self)
  {
    triple = ColorToHSV(~theme::background_color);
    triple.x *= DEG2RAD;
    popup_rect = find_valid_rect(COLOR_MENU_W, COLOR_MENU_H);
  }
  void destroy(Option&) {}
  void mainbox(Option&, Rectangle r)
  {
    DrawTextEx(
      theme::fonts[0],
      "Background color",
      {r.x+theme::gpad, r.y+r.height/2.f-theme::font_size},
      theme::font_size, theme::TEXT_SPACING,
      ~theme::text_color
    );
    const float s = r.height - theme::gpad*2.f;
    const float x = r.x + r.width - s - theme::gpad;
    const float y = r.y + r.height/2.f - s/2.f;
    DrawRectangleRec({x,y,s,s}, ~theme::background_color);
    DrawRectangleLinesEx({x,y,s,s}, theme::border_weight, ~theme::text_color);
  }
  Rectangle popup(Option& self)
  {
    color_menu(popup_rect, triple);
    theme::background_color
      = ~ColorFromHSV(triple.x*RAD2DEG, triple.y, triple.z);
    return popup_rect;
  }
}

namespace text_color
{
  Vector3 triple;
  Rectangle popup_rect;
  void init(Option& self)
  {
    triple = ColorToHSV(~theme::text_color);
    triple.x *= DEG2RAD;
    popup_rect = find_valid_rect(COLOR_MENU_W, COLOR_MENU_H);
  }
  void destroy(Option&) {}
  void mainbox(Option&, Rectangle r)
  {
    DrawTextEx(
      theme::fonts[0],
      "Text color",
      {r.x+theme::gpad, r.y+r.height/2.f-theme::font_size},
      theme::font_size, theme::TEXT_SPACING,
      ~theme::text_color
    );
    const float s = r.height - theme::gpad*2.f;
    const float x = r.x + r.width - s - theme::gpad;
    const float y = r.y + r.height/2.f - s/2.f;
    DrawRectangleRec({x,y,s,s}, ~theme::text_color);
    DrawRectangleLinesEx({x,y,s,s}, theme::border_weight, ~theme::text_color);
  }
  Rectangle popup(Option& self)
  {
    color_menu(popup_rect, triple);
    theme::text_color
      = ~ColorFromHSV(triple.x*RAD2DEG, triple.y, triple.z);
    return popup_rect;
  }
}

namespace text_field_background_color
{
  Vector3 triple;
  Rectangle popup_rect;
  void init(Option& self)
  {
    triple = ColorToHSV(~theme::text_field_bg_color);
    triple.x *= DEG2RAD;
    popup_rect = find_valid_rect(COLOR_MENU_W, COLOR_MENU_H);
  }
  void destroy(Option&) {}
  void mainbox(Option&, Rectangle r)
  {
    DrawTextEx(
      theme::fonts[0],
      "Text field background color",
      {r.x+theme::gpad, r.y+r.height/2.f-theme::font_size},
      theme::font_size, theme::TEXT_SPACING,
      ~theme::text_color
    );
    const float s = r.height - theme::gpad*2.f;
    const float x = r.x + r.width - s - theme::gpad;
    const float y = r.y + r.height/2.f - s/2.f;
    DrawRectangleRec({x,y,s,s}, ~theme::text_field_bg_color);
    DrawRectangleLinesEx({x,y,s,s}, theme::border_weight, ~theme::text_color);
  }
  Rectangle popup(Option& self)
  {
    color_menu(popup_rect, triple);
    theme::text_field_bg_color
      = ~ColorFromHSV(triple.x*RAD2DEG, triple.y, triple.z);
    return popup_rect;
  }
}

namespace command_background_color
{
  Vector3 triple;
  Rectangle popup_rect;
  void init(Option& self)
  {
    triple = ColorToHSV(~theme::command_background);
    triple.x *= DEG2RAD;
    popup_rect = find_valid_rect(COLOR_MENU_W, COLOR_MENU_H);
  }
  void destroy(Option&) {}
  void mainbox(Option&, Rectangle r)
  {
    DrawTextEx(
      theme::fonts[0],
      "Command background color",
      {r.x+theme::gpad, r.y+r.height/2.f-theme::font_size},
      theme::font_size, theme::TEXT_SPACING,
      ~theme::text_color
    );
    const float s = r.height - theme::gpad*2.f;
    const float x = r.x + r.width - s - theme::gpad;
    const float y = r.y + r.height/2.f - s/2.f;
    DrawRectangleRec({x,y,s,s}, ~theme::command_background);
    DrawRectangleLinesEx({x,y,s,s}, theme::border_weight, ~theme::text_color);
  }
  Rectangle popup(Option& self)
  {
    color_menu(popup_rect, triple);
    theme::command_background
      = ~ColorFromHSV(triple.x*RAD2DEG, triple.y, triple.z);
    return popup_rect;
  }
}



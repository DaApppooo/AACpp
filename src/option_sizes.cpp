#include "option_sizes.hpp"
#include "option_font_picking.hpp"
#include "globals.hpp"
#include "theme.hpp"

// returns increment, decrement or 0
int int_selector(Rectangle r, int value)
{
  const float selector_btn_w = theme::gpad*2.f
                                    + theme::font_size;
  const float selector_w =
    selector_btn_w*2.f + 3.f*theme::font_size;
  const float selector_h = theme::font_size + 2.f*theme::gpad;
  const Rectangle selector = {
    r.x+r.width - theme::gpad - selector_w,
    r.y + r.height/2.f - selector_h/2.f,
    selector_w, selector_h
  };
  const Rectangle btn_decr = {
    selector.x, selector.y,
    selector_btn_w, selector_h
  };
  DrawRectangleRec(
    btn_decr,
    Fade(~theme::text_color, ctrl::hover(btn_decr) ? 0.3f : 0.2f)
  );
  draw_text_anchored(
    theme::fonts[0], "-",
    {0.5f, 0.5f},
    center(btn_decr),
   theme::font_size, theme::TEXT_SPACING, ~theme::text_color
  );
  const Rectangle btn_incr = {
    selector.x+selector.width-selector_btn_w,
    selector.y,
    selector_btn_w, selector_h
  };
  DrawRectangleRec(
    btn_incr,
    Fade(~theme::text_color, ctrl::hover(btn_incr) ? 0.3f : 0.2f)
  );
  draw_text_anchored(
    theme::fonts[0], "+",
    {0.5f, 0.5f},
    center(btn_incr),
   theme::font_size, theme::TEXT_SPACING, ~theme::text_color
  );
  const Rectangle info = {
    btn_decr.x+btn_decr.width,
    selector.y,
    selector.width - selector_btn_w*2.f,
    selector.height
  };
  DrawRectangleRec(
    info,
    Fade(~theme::text_color, 0.1f)
  );
  draw_text_anchored(
    theme::fonts[0], fmt(value),
    {0.5f, 0.5f},
    center(info),
    theme::font_size,
    theme::TEXT_SPACING,
    ~theme::text_color
  );
  if (ctrl::hover(btn_decr) && ctrl::clicked())
    return -1;
  else if (ctrl::hover(btn_incr) && ctrl::clicked())
    return 1;
  return 0;
}

namespace font_size
{
  void font_reload()
  {
    theme::update_font(
      LoadFontEx(font_picking::path, theme::font_size, NULL, 0)
    );
  }
  void mainbox(Option& self, Rectangle r)
  {
    DrawTextEx(
      theme::fonts[0],
      "Current font size",
      {r.x+theme::gpad, r.y+r.height/2.f-theme::font_size},
      theme::font_size, theme::TEXT_SPACING,
      ~theme::text_color
    );
    const int res = int_selector(r, theme::font_size);
    if (res)
    {
      theme::font_size += res*2;
      font_reload();
    }
  }
}

namespace border_weight
{
  void mainbox(Option& self, Rectangle r)
  {
    DrawTextEx(
      theme::fonts[0],
      "Current border weight",
      {r.x+theme::gpad, r.y+r.height/2.f-theme::font_size},
      theme::font_size, theme::TEXT_SPACING,
      ~theme::text_color
    );
    theme::border_weight += int_selector(r, theme::border_weight);
    if (theme::border_weight > 8)
      theme::border_weight = 8;
    if (theme::border_weight < 0)
      theme::border_weight = 0;
  }
}

namespace padding
{
  void mainbox(Option& self, Rectangle r)
  {
    DrawTextEx(
      theme::fonts[0],
      "Current interface padding",
      {r.x+theme::gpad, r.y+r.height/2.f-theme::font_size},
      theme::font_size, theme::TEXT_SPACING,
      ~theme::text_color
    );
    theme::gpad += int_selector(r, theme::gpad);
    if (theme::gpad > 24)
      theme::gpad = 24;
  }
}

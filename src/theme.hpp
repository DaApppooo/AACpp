#include "clay.h"

namespace theme
{
  constexpr Clay_Color background_color = { 53, 53, 53, 255 };
  constexpr Clay_Color cell_color = { 117, 117, 117, 255 };
  constexpr Clay_Color text_color = { 242, 233, 228, 255 };
  // constexpr Clay_Color text_color = { 53, 53, 53, 255 };
  constexpr Clay_Sizing GROW = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() };
  constexpr Clay_Sizing CELL_SIZE = { CLAY_SIZING_FIXED(345), CLAY_SIZING_FIXED(200) };
  constexpr Clay_Sizing BAR_SIZE = { CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(200) };
  constexpr Clay_RectangleElementConfig TRANSPARENT_RECT = { .color = { 0, 0, 0, 0 } };
  constexpr uint16_t gpad = 32; // Global padding
  constexpr uint16_t FONT_SIZE = 40;
  constexpr uint16_t TEXT_SPACING = 2;
}

inline constexpr Clay_Sizing clay_size(int w, int h)
{
  return {float(w), float(h)};
}

// shouldn't be there


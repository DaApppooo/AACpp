#ifndef H_THEME
#define H_THEME
#define NO_CLAY
#include "clay.h"

namespace theme
{
  extern Clay_Color background_color;
  extern Clay_Color text_color;
  extern uint16_t gpad;
  extern float border_weight;
  extern Clay_Color command_background;
  extern uint16_t font_size;
  extern Clay_Color text_field_bg_color;
  extern float text_space_width; // SET BY RESMAN.cpp, NOT BY THEME.cpp
  // constexpr Clay_Color text_color = { 53, 53, 53, 255 };
  constexpr Clay_Sizing GROW = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() };
  // constexpr Clay_Sizing CELL_SIZE = { CLAY_SIZING_FIXED(345), CLAY_SIZING_FIXED(200) };
  constexpr Clay_Sizing ICON_SIZE = { CLAY_SIZING_FIXED(100), CLAY_SIZING_FIXED(100) };
  constexpr Clay_Sizing BAR_SIZE = { CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(130) };
  constexpr Clay_Dimensions IMG_SCALE = { 512.f, 512.f };
  constexpr Clay_RectangleElementConfig TRANSPARENT_RECT = { .color = { 0, 0, 0, 0 } };
  constexpr uint16_t TEXT_SPACING = 2;
}

inline constexpr Clay_Sizing clay_size(int w, int h)
{
  return {float(w), float(h)};
}

// load or reload
void theme_load();

// shouldn't be there
#endif // H_THEME

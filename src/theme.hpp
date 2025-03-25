#ifndef H_THEME
#define H_THEME
#include "utils.hpp"
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
  extern Font* fonts;
  // constexpr Clay_Color text_color = { 53, 53, 53, 255 };
  constexpr Clay_Sizing GROW = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() };
  // constexpr Clay_Sizing CELL_SIZE = { CLAY_SIZING_FIXED(345), CLAY_SIZING_FIXED(200) };
  constexpr Clay_Sizing ICON_SIZE = { CLAY_SIZING_FIXED(100), CLAY_SIZING_FIXED(100) };
  constexpr float BAR_HEIGHT = 130;
  constexpr float SAFETY_MARGIN = 50;
  constexpr Clay_Dimensions IMG_SCALE = { 512.f, 512.f };
  constexpr Clay_Color TRANSPARENT_RECT = { 0, 0, 0, 0 };
  constexpr Clay_LayoutConfig OPTION_LAYOUT = {
    .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(100) },
    .padding = { 16, 16 },
    .childAlignment = {CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER},
    .layoutDirection = CLAY_LEFT_TO_RIGHT
  };
  constexpr uint16_t TEXT_SPACING = 2;
}

inline Clay_TextElementConfig* get_font_config()
{
  static Clay_TextElementConfig buf;
  buf = {
    .textColor = theme::text_color,
    .fontId = 0,
    .fontSize = theme::font_size,
    .letterSpacing = theme::TEXT_SPACING,
    .lineHeight = 0,
    .textAlignment = CLAY_TEXT_ALIGN_RIGHT
  };
  return &buf;
}

constexpr inline Clay_String to_clay_string(const char* cstr)
{
  return {str_len(cstr), cstr};
}


// load or reload
void theme_load();

// shouldn't be there
#endif // H_THEME

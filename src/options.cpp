#include "options.hpp"
#include "utils.hpp"
#define NO_CLAY
#include "clay.h"
#include "resman.hpp"
#include "theme.hpp"
#include <raylib.h>
#include "./rlclay.h"
#include "../cxml/ui.hpp"

bool options_open;

constexpr inline float max(float a, float b)
{
  // being explicit for g++
  if (a > b) return a;
  else return b;
}

void init_options()
{
  theme_load();
}

#define NEW_OPTION(ID, TITLE, RIGHT_CONTENT) \
CLAY( \
  CLAY_ID(ID), \
  theme::TRANSPARENT_RECT, \
  theme::OPTION_LAYOUT \
) { \
 CLAY_TEXT(CLAY_STRING(TITLE), CLAY_TEXT_CONFIG(font)); \
 RIGHT_CONTENT \
}

void options_update(Clay_RenderCommandArray& render_cmds)
{
  if (!options_open)
    return;
  constexpr float MAX_WIDTH = 800.f;
  const float W = max(XMAX, MAX_WIDTH);
  const Rectangle rect = {
    XMAX/2.f - W/2.f, 0.f,
    W, YMAX
  };
  layout_options(render_cmds);
  BeginDrawing();
    Clay_Raylib_Render(render_cmds, theme::fonts);
  EndDrawing();
}

void destroy_options()
{
}


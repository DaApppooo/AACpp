#include "options.hpp"
#include "clay.h"
#include "resman.hpp"
#include "theme.hpp"
#include "utils.hpp"
#include <raylib.h>
#include "./rlclay.h"

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

  Clay_BeginLayout();

  CLAY( // body
    CLAY_RETANGLE({ .color = theme::background_color }),
    CLAY_LAYOUT({
                  .sizing = theme::GROW,
                  .layoutDirection = CLAY_LEFT_TO_RIGHT
                })
  ) {
    CLAY( // margin left
      theme::TRANSPARENT_RECT,
      CLAY_LAYOUT({
        .sizing = {
          .width=CLAY_SIZING_FIXED(rect.x),
          .height=CLAY_SIZING_GROW()
        }
      })
    ) {}
    CLAY( // content
      theme::TRANSPARENT_RECT,
      CLAY_LAYOUT({
        .sizing = {
          .width=CLAY_SIZING_FIXED(rect.width),
          .height=CLAY_SIZING_GROW()
        }
      })
    ) {
      // List here all options:
      NEW_OPTION(
                 "DEFAULT_BOARD",
                 "Select the default board to be loaded without asking.",
               );
    }
    CLAY( // margin right
      theme::TRANSPARENT_RECT,
      CLAY_LAYOUT({
        .sizing = {
          .width=CLAY_SIZING_FIXED(rect.x),
          .height=CLAY_SIZING_GROW()
        }
      })
    ) {}
  }

  render_cmds = Clay_EndLayout();

  BeginDrawing();
    Clay_Raylib_Render(render_cmds);
    DrawTexturePro(
      btns[BTI_BACKSPACE],
      {
        0.f, 0.f,
        (float)btns[BTI_BACKSPACE].width,
        (float)btns[BTI_BACKSPACE].height
      },
      {
        (float)theme::gpad, (float)theme::gpad,
        theme::IMG_SCALE.width,
        theme::IMG_SCALE.height
      },
      {0.f, 0.f},
      0.f,
      WHITE
    );
  EndDrawing();
}

void destroy_options()
{
}


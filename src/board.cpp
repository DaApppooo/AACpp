#include "board.hpp"
#include "resman.hpp"
#include "theme.hpp"
#include <cassert>
#include <raylib.h>
#include "rlclay.h"
#include "utils.hpp"

void Board::serialize(Stream f)
{
  f.write(layout_width);
  f.write(layout_height);
  for (int i = 0; i < layout_width*layout_height; i++)
  {
    cells[i].serialize(f);
  }
  // parent is obvious based on how hierarchy is organized linearly
}
void Board::deserialize(Stream f)
{
  char buf[3];
  f.read(buf, 3);
  assert(memcmp(buf, "BRD", 3) == 0);
  layout_width = f.read<int>();
  layout_height = f.read<int>();
  cells = (Cell*)malloc(sizeof(Cell)*layout_width*layout_height);
  for (int i = 0; i < layout_width*layout_height; i++)
  {
    cells[i].init();
    cells[i].rect = cells[i].get_rect(*this, i%layout_width, i/layout_width);
    cells[i].deserialize(f);
  }
}
void Board::draw()
{
  {
    // Draw actions
    float x = theme::gpad;
    for (int i = 0; i < current_actions.len(); i++)
    {
      if (current_actions[i]._data[0] != '+')
      {
        todo();
      }
      const char* buf = current_actions[i]._data+1;
      const float w = MeasureTextEx(
        Raylib_fonts[0].font,
        buf,
        theme::FONT_SIZE,
        theme::TEXT_SPACING
      ).x;
      if (x + w > XMAX-theme::gpad)
        // TODO: Add a button to let the user see other available actions.
        break;
      DrawTextEx(
        Raylib_fonts[0].font,
        buf,
        {
          x,
          theme::BAR_SIZE.height.sizeMinMax.min + theme::gpad
        },
        theme::FONT_SIZE,
        theme::TEXT_SPACING,
        CLAY_COLOR_TO_RAYLIB_COLOR(theme::text_color)
      );
      x += w + theme::gpad;
    }
  }

  for (int y = 0; y < layout_height; y++)
  {
    for (int x = 0; x < layout_width; x++)
    {
      cells[x + y*layout_width].draw(*this, x, y);
    }
  }
}

opt_board_index_t Board::update()
{
  opt_board_index_t p;
  {
    // Draw cells
    p = INVALID_BOARD_INDEX;
    for (int y = 0; y < layout_height; y++)
    {
      for (int x = 0; x < layout_width; x++)
      {
        const opt_board_index_t np =
          cells[x + y*layout_width].update(*this, x, y);
        if (is_board_index(np))
          p = np; // MATHEMATICIANS IN SHAMBLES !!! I SOLVED IT GUYS !!! /j
      }
    }
  }
  return p;
}
void Board::destroy()
{
  for (int i = 0; i < layout_width * layout_height; i++)
  {
    cells[i].destroy();
  }
  free(cells);
  layout_width = layout_height = 0;
}


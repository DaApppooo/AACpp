#include "board.hpp"
#include "resman.hpp"
#include "theme.hpp"
#include <cassert>
#include "rlclay.h"
#include "utils.hpp"
#include "globals.hpp"
#include <raylib.h>

list<board_index_t> parents;
void push_parent_board(board_index_t idx)
{
  parents.push(idx);
}
opt_board_index_t pop_parent_board()
{
  if (parents.len() > 0)
    return parents.pop();
  else
    return INVALID_BOARD_INDEX;
}

void Board::serialize(Stream f)
{
  f << (int&) layout_width;
  f << (int&) layout_height;
  for (int i = 0; i < layout_width*layout_height; i++)
  {
    cells[i].serialize(f);
  }
}
void Board::deserialize(Stream f)
{
  char buf[4];
  f.check_anchor("BRD");
  f >> (int&) layout_width;
  f >> (int&) layout_height;
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
    for (int i = 0; i < current_actions.len; i++)
    {
      if (current_actions[i]._data[0] != '+')
      {
        todo();
      }
      const char* buf = current_actions[i]._data+1;
      if (*buf == ' ') buf++;
      const float w = MeasureTextEx(
        theme::fonts[0],
        buf,
        theme::font_size,
        theme::TEXT_SPACING
      ).x;
      if (x + w > XMAX-theme::gpad)
        // TODO: Add a button to let the user see other available actions.
        break;
      DrawRectangleRounded(
        {
          x + theme::gpad,
          theme::BAR_HEIGHT + theme::gpad/2.f,
          w + theme::text_space_width + theme::gpad*2.f,
          theme::font_size+5.f + theme::gpad/2.f
        },
        0.75f,
        16,
        CLAY_COLOR_TO_RAYLIB_COLOR(theme::text_field_bg_color)
      );
      DrawTextEx(
        theme::fonts[0],
        buf,
        {
          x + theme::gpad + theme::text_space_width,
          theme::BAR_HEIGHT + theme::gpad
        },
        theme::font_size,
        theme::TEXT_SPACING,
        CLAY_COLOR_TO_RAYLIB_COLOR(theme::text_color)
      );
      x += w + theme::gpad*3.f;
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


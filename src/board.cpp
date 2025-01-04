#include "board.hpp"
#include "resman.hpp"
#include <cassert>

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
    cells[i].name = nullptr; 
    cells[i].rect = cells[i].get_rect(*this, i%layout_width, i/layout_width);
    cells[i].deserialize(f);
  }
}
void Board::draw()
{
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
  opt_board_index_t p = INVALID_BOARD_INDEX;
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


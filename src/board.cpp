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
  layout_width = f.read<int>();
  layout_height = f.read<int>();
  cells = (Cell*)malloc(sizeof(Cell)*layout_width*layout_height);
  for (int i = 0; i < layout_width*layout_height; i++)
  {
    cells[i].deserialize(f);
  }
}
void Board::draw()
{
  for (int y = 0; y < layout_height; y++)
  {
    for (int x = 0; x < layout_width; x++)
    {
      cells[x + y*layout_width].draw(x, y);
    }
  }
}

Board* Board::update()
{
  Board* p = nullptr;
  for (int y = 0; y < layout_height; y++)
  {
    for (int x = 0; x < layout_width; x++)
    {
      Board* const np = cells[x + y*layout_width].update(x, y);
      assert(!(np != nullptr && p != nullptr));
      p = np;
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


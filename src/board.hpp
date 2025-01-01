#ifndef H_BOARD
#define H_BOARD
#include "resman.hpp"
#include "utils.hpp"
#include "cell.hpp"
#pragma once

struct Board
{
  // Board Builder usual layout
  int layout_width;
  int layout_height;
  
  Cell* cells; // linrect layout (allocated size is width*height)

  void init(opt_board_index_t parent, int w, int h)
  {
    layout_width = w;
    layout_height = h;
    cells = (Cell*)malloc(sizeof(Cell)*w*h);
    for (int i = 0; i < layout_width * layout_height; i++)
    {
      cells[i].init(parent);
    }
  }
  void serialize(Stream f);
  void deserialize(Stream f);
  Board* update();
  void draw();
  void destroy();
};
#endif

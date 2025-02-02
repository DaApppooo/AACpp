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

  inline void hold()
  {
    for (int i = 0; i < layout_width*layout_height; i++)
    {
      cells[i].hold();
    }
  }
  inline void drop()
  {
    for (int i = 0; i < layout_width*layout_height; i++)
    {
      cells[i].drop();
    }
  }
  void serialize(Stream f);
  void deserialize(Stream f);
  opt_board_index_t update();
  void draw();
  void destroy();
};
#endif

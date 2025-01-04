#include "resman.hpp"
#include "theme.hpp"
#include "utils.hpp"
#include "board.hpp"
#define CLAY__MIN(x, y) ((x < y) ? (x) : (y))
#define CLAY__MAX(x, y) ((x > y) ? (x) : (y))
#include "rlclay.h"
#include <raylib.h>

void Cell::destroy()
{
  if (name != nullptr)
  {
    free(name);
    name = nullptr;
  }
}

Rectangle Cell::get_rect(const Board& board, int x, int y) const
{
  return {
    x * ((XMAX - theme::gpad*2)/board.layout_width) + theme::gpad,
    y * (
          (YMAX - theme::gpad*2 - theme::BAR_SIZE.height.sizeMinMax.min)
          /board.layout_height
        )
      + theme::gpad
      + theme::BAR_SIZE.height.sizeMinMax.min,
    ((XMAX - theme::gpad*2)/board.layout_width) - theme::gpad, // these -gpads
    ((YMAX - theme::gpad*2 - theme::BAR_SIZE.height.sizeMinMax.min)/board.layout_height) - theme::gpad // are margins
  };
}


void Cell::draw(const Board& board, int x, int y) const
{
  // faster to recompute everytime
  const bool inbounds = CheckCollisionPointRec(ctrl::mpos, rect);
  DrawRectangleRec(
    rect,
    CLAY_COLOR_TO_RAYLIB_COLOR(theme::cell_color)
  );
  Vector2 txtm = {0.0f, theme::FONT_SIZE};
  if (str_len(name) != 0)
  {
    txtm = MeasureTextEx(
      Raylib_fonts[0].font,
      name,
      theme::FONT_SIZE,
      theme::TEXT_SPACING
    );
    DrawTextEx(
               Raylib_fonts[0].font,
               name,
               {
                rect.x + rect.width/2.f - txtm.x/2.f,
                rect.y + rect.height - txtm.y
             },
               theme::FONT_SIZE,
               theme::TEXT_SPACING,
               CLAY_COLOR_TO_RAYLIB_COLOR(theme::text_color)
             );
  }
  if (tex_id != -1)
  {
    const float w = texs[tex_id].width;
    const float h = texs[tex_id].height;
    const float ratio = w/h;
    DrawTexturePro(
      texs[tex_id],
      {0, 0, w, h},
      {
        rect.x + rect.width/2.f - (ratio*(rect.height-txtm.y))/2.f,
        rect.y,
        ratio*(rect.height-txtm.y),
        rect.height - txtm.y
      },
      {0,0},
      0,
      WHITE
    );
  }
}

/*
All of this will be simplified when Clay actually implements
grids and flex-wrap containers.
*/
opt_board_index_t Cell::update(const Board& board, int x, int y) {
  // faster to recompute everytime
  const bool inbounds = CheckCollisionPointRec(ctrl::mpos, rect);
  const Rectangle ideal_rect = get_rect(board, x, y);
  const float lerp_speed = 0.1f;
  rect.x = Lerp(rect.x, ideal_rect.x, lerp_speed);
  rect.y = Lerp(rect.y, ideal_rect.y, lerp_speed);
  rect.width = Lerp(rect.width, ideal_rect.width, lerp_speed);
  rect.height = Lerp(rect.height, ideal_rect.height, lerp_speed);
  if (is_folder()) {
    if (ctrl::mouse_down && inbounds) {
      return child;
    }
  } else {
    if (ctrl::mouse_down && inbounds) {
      if (spring) {
        if (str_len(name) != 0) {
          tts_push_word(name);
        }
        spring = false;
      }
    } else {
      spring = true;
    }
  }
  return INVALID_BOARD_INDEX;
}

void Cell::to_folder(board_index_t current_board)
{
  child = alloc_board();
}

void Cell::serialize(Stream f)
{
  // const isize l = str_len(name);
  // f.write(tex_id);
  // f.write(l);
  // f.write(name, l);
  // f.write(parent);
  // f.write(child);
  todo();
}

void Cell::deserialize(Stream f)
{
  assert(sizeof(isize) == 8);
  char buf[3];
  isize l;
  f.read(buf, 3);
  assert(memcmp(buf, "CLL", 3) == 0);
  tex_id = f.read<int>();
  l = f.read<isize>();
  assert(l >= 0);
  name = (char*)malloc(sizeof(char)*(l+1));
  f.read(name, l);
  name[l] = 0;
  parent = f.read<opt_board_index_t>();
  child = f.read<opt_board_index_t>();
}


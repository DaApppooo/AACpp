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
  name.destroy();
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
    {
      rect.x-theme::border_weight, rect.y-theme::border_weight,
      rect.width + theme::border_weight*2.f,
      rect.height + theme::border_weight*2.f
    },
    border // this is now default CLAY_COLOR_TO_RAYLIB_COLOR(theme::cell_color)
  );
  DrawRectangleRec(
    rect,
    background // this is now default CLAY_COLOR_TO_RAYLIB_COLOR(theme::cell_color)
  );
  Vector2 txtm = {0.0f, theme::FONT_SIZE};
  if (name.len() != 0)
  {
    const char* buf = fix2var_utf8(name, (char*)TextFormat(""));
    txtm = MeasureTextEx(
      Raylib_fonts[0].font,
      buf,
      theme::FONT_SIZE,
      theme::TEXT_SPACING
    );
    DrawTextEx(
               Raylib_fonts[0].font,
               buf,
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
All of this will be simplified when Clay actually implements grids and
flex-wrap containers.
Although this would disable absolute positioning as described in the obf format
specification.
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
        if (name.len() != 0) {
          tts_push(name);
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
  f.read(buf, 3);
  assert(memcmp(buf, "CLL", 3) == 0);
  tex_id = f.read<int>();
  name.deserialize(f._f);
  parent = f.read<opt_board_index_t>();
  child = f.read<opt_board_index_t>();
  background = f.read<Color>();
  border = f.read<Color>();
}


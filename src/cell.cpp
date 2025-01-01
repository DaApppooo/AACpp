#include "resman.hpp"
#include "theme.hpp"
#include "utils.hpp"
#include "board.hpp"
#include "rlclay.h"
#include <raylib.h>


void Cell::draw(const Board& board, int x, int y) const
{
  // faster to recompute everytime
  const Rectangle rect = {
    x * ((XMAX - theme::gpad*2)/board.layout_width) + theme::gpad,
    y * ((YMAX - theme::gpad*2)/board.layout_height) + theme::gpad,
    ((XMAX - theme::gpad*2)/board.layout_width) - theme::gpad, // these -gpads
    ((YMAX - theme::gpad*2)/board.layout_height) - theme::gpad // are margins
  };
  const bool inbounds = CheckCollisionPointRec(ctrl::mpos, rect);
  DrawRectangleRounded(
    rect, 5.f, 16,
    CLAY_COLOR_TO_RAYLIB_COLOR(theme::cell_color)
  );
  if (!name.is_empty())
  {
    const Vector2 m = MeasureTextEx(
      Raylib_fonts[0].font,
      name.data(),
      theme::FONT_SIZE,
      theme::TEXT_SPACING
    );
    DrawTextEx(
               Raylib_fonts[0].font,
               name.data(),
               {
                rect.x + rect.width/2.f - m.x/2.f,
                rect.y + rect.height - m.y
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
    const float ratio = h/w;
    // DrawTexturePro(, , , , , )
    todo();
  }
}

/*
All of this will be simplified when Clay actually implements
grids and flex-wrap containers.
*/
opt_board_index_t Cell::update(const Board& board, int x, int y) {
  // faster to recompute everytime
  const Rectangle rect = {
    x * (XMAX/board.layout_width) + theme::gpad,
    y * (YMAX/board.layout_height) + theme::gpad,
    (XMAX/board.layout_width),
    (YMAX/board.layout_height)
  };
  const bool inbounds = CheckCollisionPointRec(ctrl::mpos, rect);
  // SPEAK MODE (non-edit mode)
  if (is_folder()) {
    if (ctrl::mouse_down && inbounds) {
      return child;
    }
  } else {
    if (ctrl::mouse_down && inbounds) {
      if (spring) {
        if (!name.is_empty()) {
          tts_push_word(name.data());
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
  isize l = name.len();
  f.write(tex_id);
  f.write(l);
  f.write(name.data(), name.len());
  f.write(parent);
  f.write(child);
}

void Cell::deserialize(Stream f)
{
  isize l;
  tex_id = f.read<int>();
  l = f.read<isize>();
  name.prealloc(l+1);
  f.read(name.data(), l);
  name.data()[l] = 0;
  parent = f.read<opt_board_index_t>();
  child = f.read<opt_board_index_t>();
}


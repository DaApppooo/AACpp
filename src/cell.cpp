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
          (YMAX - theme::gpad*2 - theme::BAR_SIZE.height.sizeMinMax.min*1.5f)
          /board.layout_height
        )
      + theme::gpad
      + theme::BAR_SIZE.height.sizeMinMax.min*1.5f,
    ((XMAX - theme::gpad*2)/board.layout_width) - theme::gpad, // these -gpads
    (
      (YMAX - theme::gpad*2 - theme::BAR_SIZE.height.sizeMinMax.min*1.5f)
      /board.layout_height
    ) - theme::gpad // are margins, and not paddings, but anyway
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
  if (is_folder())
  {
    DrawRectangleRec(
      {
        rect.x+rect.width-30.f, rect.y,
        30.f,
        20.f
      },
      border // this is now default CLAY_COLOR_TO_RAYLIB_COLOR(theme::cell_color)
    );
  }
  Vector2 txtm = {0.0f, (float)theme::font_size};
  if (name.len() != 0)
  {
    constexpr float SCALE_DOWN_STEP_FACTOR = 1.2f; // MUST BE STRICTLY OVER 1
    const char* buf = name._data;
    float font_size = theme::font_size*SCALE_DOWN_STEP_FACTOR;
    do {
      font_size /= SCALE_DOWN_STEP_FACTOR;
      txtm = MeasureTextEx(
        Raylib_fonts[0].font,
        buf,
        font_size,
        theme::TEXT_SPACING
      );
    } while (txtm.x > rect.width);
    DrawTextEx(
      Raylib_fonts[0].font,
      buf,
      {
        rect.x + rect.width/2.f - txtm.x/2.f,
        rect.y + rect.height - txtm.y
      },
      font_size,
      theme::TEXT_SPACING,
      CLAY_COLOR_TO_RAYLIB_COLOR(theme::text_color)
    );
  }
  if (tex_id != -1)
  {
    const float ratio = texs[tex_id].rect.width/texs[tex_id].rect.height;
    texs[tex_id].draw(
      {
        rect.x + rect.width/2.f - (ratio*(rect.height-txtm.y-theme::gpad))/2.f,
        rect.y + theme::gpad,
        ratio*(rect.height-txtm.y-theme::gpad),
        rect.height - txtm.y - theme::gpad
      }
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
  const float lerp_speed = 1.f;
  rect.x = Lerp(rect.x, ideal_rect.x, lerp_speed);
  rect.y = Lerp(rect.y, ideal_rect.y, lerp_speed);
  rect.width = Lerp(rect.width, ideal_rect.width, lerp_speed);
  rect.height = Lerp(rect.height, ideal_rect.height, lerp_speed);
  if (name._byte_len == 0 || actions.len() == 0)
    return INVALID_BOARD_INDEX;
  if (is_folder())
  {
    if (ctrl::touch_press && inbounds)
    {
      current_actions.init();
      return child;
    }
  } else {
    if (ctrl::touch_press && inbounds)
    {
      if (actions[0]._data[0] == '+')
      {
        tts_push(actions[0]);
        current_actions.assign(actions);
      }
      else
        TraceLog(LOG_WARNING, "Non + actions aren't supported yet.");
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
  i64 size;
  f.read(buf, 3);
  if (memcmp(buf, "CLL", 3) != 0)
  {
    TraceLog(
      LOG_ERROR,
      "Miss alignement, expected to read CLL, read %c%c%c instead (position %p)",
      buf[0], buf[1], buf[2],
      (void*)(ptrdiff_t)(ftell(f._f) - 3)
    );
    abort();
  }
  tex_id = f.read<int>();
  name.deserialize(f._f);
  size = f.read<i64>();
  actions.prealloc(size);
  actions.set_len(size);
  for (int i = 0; i < size; i++)
  {
    actions[i].deserialize(f._f);
  }
  parent = f.read<opt_board_index_t>();
  child = f.read<opt_board_index_t>();
  background = f.read<Color>();
  border = f.read<Color>();
}


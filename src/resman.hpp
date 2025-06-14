#ifndef H_RESMAN
#define H_RESMAN
#define NO_CLAY
#include "clay.h"
#include "list.hpp"
#include "utils.hpp" // for windows compat., ensure this is before raylib
#include <raylib.h>

using String = list<char>;
extern FILE* source_cobz;
extern Clay_TextElementConfig font;
extern "C"
{ extern Texture btns[5]; }
extern View<FixedString> current_actions;
extern "C" {
void layout_home(Clay_RenderCommandArray& array);
void layout_options(Clay_RenderCommandArray& array);
}

void tex_hold(int ssid);
void tex_drop(int ssid);
struct TexRect
{
  Rectangle rect;
  void draw(Rectangle target);
};
extern list<TexRect> rects;

enum BtnRectId
{
  BTI_UP,
  BTI_BACKSPACE,
  BTI_CLEAR,
  BTI_PLAY,
  BTI_OPT
};

using board_index_t = uint32_t;
using opt_board_index_t = int32_t;
constexpr opt_board_index_t INVALID_BOARD_INDEX = -1;

struct Board;

void TextureDumpSave(Texture tex, Stream s);
void TextureDumpLoad(Texture& tex, Stream s);

opt_board_index_t board_index_from_pointer(const Board* const b);
Board& board_with_index(board_index_t bi);
Board& board_with_index(opt_board_index_t obi) = delete;
inline bool is_board_index(opt_board_index_t obi)
{
  return obi >= 0;
}

void init_res();
int res_load_boardset(Stream s);
void reset_res();
void destroy_res();

#endif

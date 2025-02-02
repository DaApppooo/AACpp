#ifndef H_RESMAN
#define H_RESMAN
#define NO_CLAY
#include "clay.h"
#include "list.hpp"
#include <raylib.h>

using String = list<char>;
struct TextureCargo
{
  Texture tex;
  int32_t seek : 30;
  bool ideal_state : 1; // ideal state for loaded, used to give direction
                        // to the loading process
  bool loaded : 1;
};

extern FILE* source_cobz;
extern list<TextureCargo> texs;
extern Clay_TextElementConfig font;
extern Texture btns[5];
extern Ref<list<FixedString>> current_actions;
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
void hold_tex(int valid_tex_id);
void drop_tex(int valid_tex_id);

board_index_t alloc_board();
opt_board_index_t board_index_from_pointer(const Board* const b);
Board& board_with_index(board_index_t bi);
Board& board_with_index(opt_board_index_t obi) = delete;
inline bool is_board_index(opt_board_index_t obi)
{
  return obi >= 0;
}



int init_res(Stream s);

void destroy_res();
#endif

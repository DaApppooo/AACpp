#ifndef H_RESMAN
#define H_RESMAN
#define NO_CLAY
#include "clay.h"
#include "list.hpp"
#include <raylib.h>

using String = list<char>;
extern list<Texture> texs;
extern Clay_TextElementConfig font;

using board_index_t = uint32_t;
using opt_board_index_t = int32_t;
constexpr opt_board_index_t INVALID_BOARD_INDEX = -1;

struct Board;

void TextureDumpSave(Texture tex, Stream s);
void TextureDumpLoad(Texture& tex, Stream s);

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

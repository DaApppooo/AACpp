#include "utils.hpp"
#include <raylib.h>
#include "resman.hpp"
#include "theme.hpp"

struct Board;

struct Cell
{
  Rectangle rect;
  FixedString name;
  opt_board_index_t child;
  opt_board_index_t parent;
  int tex_id;
  Color background;
  Color border;
  bool spring;

  inline bool is_folder() const
  {
    return is_board_index(child);
  }
  inline bool is_word() const
  {
    return !is_board_index(child);
  }
  Rectangle get_rect(const Board& board, int x, int y) const;
  void to_folder(board_index_t current_board);
  void serialize(Stream f);
  void deserialize(Stream f);
  opt_board_index_t update(const Board& board, int x, int y);
  void draw(const Board& board, int x, int y) const;
  void destroy();
};


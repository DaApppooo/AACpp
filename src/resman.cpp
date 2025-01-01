#include "resman.hpp"
#include "board.hpp"
#include <cstdlib>
#include <raylib.h>
#include <stb/stb_image.h>

list<Texture> texs;
list<Board> boards;

void TextureDumpLoad(Texture& tex, Stream s)
{
  static char buf[6];
  int c, buf_pos, channels;
  isize size;
  Image temp;
  s.read(buf, 3);
  assert(memcmp(buf, "IMG", 3) == 0); // if this fails, you're not reading a texture
  temp.data = stbi_load_from_file(s._f, &temp.width, &temp.height, &channels, 0);
  
  if (temp.data == nullptr)
  {
    TraceLog(LOG_ERROR, "Failed to read one image from file.");
    return;
  }

  // this shit comes from raylib
  temp.mipmaps = 1;
  if (channels == 1) temp.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
  else if (channels == 2) temp.format = PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA;
  else if (channels == 3) temp.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
  else if (channels == 4) temp.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

  tex = LoadTextureFromImage(temp);
  UnloadImage(temp);
  TraceLog(LOG_INFO, "Successfuly loaded one texture from file !");
}

board_index_t alloc_board()
{
  boards.push(Board{});
  return boards.len()-1;
}

opt_board_index_t board_index_from_pointer(const Board* const b)
{
  if (b < boards.data() || b >= boards.data() + boards.len())
  {
    return INVALID_BOARD_INDEX;
  }
  return b - boards.data();
}

Board& board_with_index(board_index_t bi)
{ return boards[bi]; }

int init_res(Ref<Stream> s)
{
  isize tex_count, board_count;
  
  texs.init();
  boards.init();

  tex_count = s.read<isize>();
  texs.prealloc(tex_count);
  for (isize i = 0; i < tex_count; i++)
  {
    Texture tex;
    TextureDumpLoad(tex, s);
    texs.push(tex);
  }

  board_count = s.read<isize>();
  boards.prealloc(board_count);
  for (isize i = 0; i < board_count; i++)
  {
    Board b;
    b.deserialize(s);
    boards.push(b);
  }

  return EXIT_SUCCESS;
}

void destroy_res()
{
  texs.destroy();
  boards.destroy();
}

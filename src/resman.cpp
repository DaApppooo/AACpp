#include "resman.hpp"
#include "board.hpp"
#include "list.hpp"
#include "proc.hpp"
#include "theme.hpp"
#include "utils.hpp"
#include <cstdint>
#include <cstdio>
#include <unistd.h>
#define NO_CLAY
#include "clay.h"
#include <cstdlib>
#include <raylib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "rlclay.h"

Texture glob_tex;
list<TextureCargo> texs;
list<Board> boards;
Clay_TextElementConfig font;
Texture btns[5];
FILE* source_cobz;

Ref<list<FixedString>> current_actions;

void TextureDumpLoad(Image& temp, Stream s)
{
  static char buf[3];
  int channels;
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
  else TraceLog(LOG_WARNING, "Unknown channel-format relation for channels=%i", channels);

  TraceLog(LOG_INFO, "Successfuly loaded one image from file !");
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

extern 
    Clay_Dimensions (*Clay__MeasureText)(
                              Clay_String *text,
                              Clay_TextElementConfig *config
                            );



int init_res(Ref<Stream> s)
{
  assert(sizeof(isize)==8);
  isize board_count, tex_count;

  source_cobz = s._f; // global set

  current_actions.init();

  Raylib_fonts[0].font = LoadFontEx("res/font.ttf", theme::font_size*2, nullptr, -1);
  font.fontId = 0;
  font.fontSize = theme::font_size;
  font.letterSpacing = theme::TEXT_SPACING;
  font.textColor = theme::text_color;
  font.lineHeight = 1;
  font.wrapMode = CLAY_TEXT_WRAP_NONE;
  Clay__MeasureText = Raylib_MeasureText;
  theme::text_space_width = MeasureTextEx(
    Raylib_fonts[0].font,
    " ",
    theme::font_size,
    theme::TEXT_SPACING
  ).x;

  btns[BTI_OPT] = LoadTexture("res/opt.png");
  btns[BTI_BACKSPACE] = LoadTexture("res/kb.png");
  btns[BTI_CLEAR] = LoadTexture("res/cl.png");
  btns[BTI_PLAY] = LoadTexture("res/pa.png");
  btns[BTI_UP] = LoadTexture("res/au.png");
  
  texs.init();
  boards.init();

  board_count = s.read<isize>();
  boards.prealloc(board_count);
  for (isize i = 0; i < board_count; i++)
  {
    Board b;
    b.deserialize(s);
    boards.push(b);
  }

  TextureDumpLoad(glob_tex, s);
  tex_count = s.read<isize>();
  texs.prealloc(tex_count);
  fread(texs.data(), sizeof(TextureCargo), tex_count, s._f);
  
  return EXIT_SUCCESS;
}

void destroy_res()
{
  UnloadFont(Raylib_fonts[0].font);
  UnloadTexture(glob_tex);
  texs.destroy();
  boards.destroy();
}

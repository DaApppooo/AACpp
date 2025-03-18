#include "resman.hpp"
#include "board.hpp"
#include "list.hpp"
#include "theme.hpp"
#include "utils.hpp"
#include <cstdio>
#include <unistd.h>
#define NO_CLAY
#include "clay.h"
#include <cstdlib>
#include <raylib.h>
#define STBI_MAX_DIMENSIONS (1<<29)
#include "spng.h"
#include "rlclay.h"

Texture glob_tex;
list<TexInfo> texs;
list<Board> boards;
Clay_TextElementConfig font;
Texture btns[5];
FILE* source_cobz;

Ref<list<FixedString>> current_actions;

void TextureDumpLoad(Texture& tex, Stream s)
{
  static char buf[3];
  Image temp;
  int channels;
  spng_ctx* ctx;
  size_t len;
  int spngerr;
  spng_ihdr ihdr;

  tex.id = tex.height = tex.width = 0;
  s.read(buf, 3);
  ctx = spng_ctx_new(0);
  // temp.data = (s._f, &temp.width, &temp.height, &channels, 0);
  spngerr = spng_set_png_file(ctx, s._f);
  if (spngerr)
  {
    TraceLog(LOG_ERROR, "[LIBSPNG] %s", spng_strerror(spngerr));
    return;
  }
  spngerr = spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &len);
  if (spngerr)
  {
    TraceLog(LOG_ERROR, "[LIBSPNG] %s", spng_strerror(spngerr));
    return;
  }
  spngerr = spng_get_ihdr(ctx, &ihdr);
  if (spngerr)
  {
    TraceLog(LOG_ERROR, "[LIBSPNG] %s", spng_strerror(spngerr));
    return;
  }
  TraceLog(
    LOG_INFO, "Image size is %ix%i (%lu bytes)",
    ihdr.width, ihdr.height,
    len
  );
  temp.width = ihdr.width;
  temp.height = ihdr.height;
  temp.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
  temp.mipmaps = 1;
  temp.data = malloc(len);
  if (temp.data == nullptr)
  {
    TraceLog(LOG_ERROR, "Not enough memory to load the spritesheet.");
    return;
  }
  spngerr = spng_decode_image(ctx, temp.data, len, SPNG_FMT_RGBA8, 0);
  if (spngerr)
  {
    free(temp.data);
    TraceLog(LOG_ERROR, "[LIBSPNG] %s", spng_strerror(spngerr));
    return;
  }
  spng_ctx_free(ctx);
    
  tex = LoadTextureFromImage(temp);
  UnloadImage(temp);
  if (tex.id != 0)
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

  tex_count = s.read<isize>();
  texs.prealloc(tex_count);
  fread(texs.data(), sizeof(TexInfo), tex_count, s._f);
  TextureDumpLoad(glob_tex, s);
    
  return EXIT_SUCCESS;
}

void destroy_res()
{
  UnloadFont(Raylib_fonts[0].font);
  UnloadTexture(glob_tex);
  texs.destroy();
  boards.destroy();
}

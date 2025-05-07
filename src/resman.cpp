#include "resman.hpp"
#include "board.hpp"
#include "list.hpp"
#include "theme.hpp"
#include "utils.hpp"
#include <cstdio>
#include <cstring>
#include <unistd.h>
#define NO_CLAY
#include "clay.h"
#include <cstdlib>
#include <raylib.h>
#define STBI_MAX_DIMENSIONS (1<<29)
#include "spng.h"
#include <GL/gl.h>
#include "rlclay.h"
#include "globals.hpp"

list<Texture> spritesheets;
list<TexInfo> texs;
list<Board> boards;
Clay_TextElementConfig font;
extern "C"
{ Texture btns[5]; }
FILE* source_cobz;

Ref<list<FixedString>> current_actions;


void TextureDumpLoad(Texture& tex, Stream s)
{
  char buf[4];
  int channels;
  spng_ctx* ctx;
  size_t len;
  int spngerr;
  spng_ihdr ihdr;
  void* data = nullptr;
  unsigned int id = 0;

  tex.id = tex.height = tex.width = 0;
  // Here is some weird shenanigans because libspng doesn't seem to read the
  // whole image data and stops before the end. Thus these alignement anchors
  // are necessary (the alignement anchor being "IMG\x00")
  s.align_until_anchor("IMG");

  ctx = spng_ctx_new(0);
  // temp.data = (s._f, &temp.width, &temp.height, &channels, 0);
  spngerr = spng_set_png_file(ctx, s._f);
  if (spngerr)
  {
    TraceLog(LOG_ERROR, "[LIBSPNG] %s", spng_strerror(spngerr));
    goto error;
  }
  spngerr = spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &len);
  if (spngerr)
  {
    TraceLog(LOG_ERROR, "[LIBSPNG] %s", spng_strerror(spngerr));
    goto error;
  }
  spngerr = spng_get_ihdr(ctx, &ihdr);
  if (spngerr)
  {
    TraceLog(LOG_ERROR, "[LIBSPNG] %s", spng_strerror(spngerr));
    goto error;
  }
  TraceLog(
    LOG_INFO, "Image size is %ix%i (%lu bytes)",
    ihdr.width, ihdr.height,
    len
  );
  data = malloc(len);
  if (data == nullptr)
  {
    TraceLog(LOG_ERROR, "Not enough memory to load the spritesheet.");
    goto error;
  }
  spngerr = spng_decode_image(ctx, data, len, SPNG_FMT_RGBA8, 0);
  if (spngerr)
  {
    TraceLog(LOG_ERROR, "[LIBSPNG] %s", spng_strerror(spngerr));
    goto error;
  }
  spng_ctx_free(ctx);
  
  glBindTexture(GL_TEXTURE_2D, 0);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGBA,
    ihdr.width,
    ihdr.height,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    data
   );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glBindTexture(GL_TEXTURE_2D, 0);

  tex.id = id;
  tex.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
  tex.height = ihdr.height;
  tex.width  = ihdr.width;
  tex.mipmaps = 1;
  
  if (tex.id > 0)
  {
    TraceLog(LOG_INFO, "Successfuly loaded one image from file !");
    return;
  }
error:
  if (data != nullptr)
    free(data);
  TraceLog(LOG_ERROR, "Failed to load image.");
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

void init_res()
{
  spritesheets.init();
  spritesheets.init();
  texs.init();
  boards.init();
  source_cobz = nullptr;
}
void reset_res()
{
  
}

int res_load_boardset(Ref<Stream> s)
{
  assert(sizeof(isize)==8);
  isize board_count, tex_count;

  source_cobz = s._f; // global set

  current_actions.init();

  // NOTE: Font is loaded with init_settings
  theme::text_space_width = MeasureTextEx(
    theme::fonts[0],
    " ",
    theme::font_size,
    theme::TEXT_SPACING
  ).x;

  btns[BTI_OPT] = LoadTexture("assets/opt.png");
  btns[BTI_BACKSPACE] = LoadTexture("assets/kb.png");
  btns[BTI_CLEAR] = LoadTexture("assets/cl.png");
  btns[BTI_PLAY] = LoadTexture("assets/pa.png");
  btns[BTI_UP] = LoadTexture("assets/au.png");
  
  texs.init();
  boards.init();

  s >> (isize&) board_count;
  boards.prealloc(board_count);
  for (isize i = 0; i < board_count; i++)
  {
    Board b;
    b.deserialize(s);
    boards.push(b);
  }

  // Rectangles
  s >> (isize&) tex_count;
  s >> (isize&) board_count;
  texs.prealloc(tex_count);
  fread(texs.data(), sizeof(TexInfo), tex_count, s._f);
  texs.set_len(tex_count);
  // Spritesheets
  const float LOADING_W = 0.1f * XMAX;
  const float LOADING_H = 0.05f * YMAX;
  assert(board_count != 0);
  spritesheets.prealloc(board_count);
  for (isize i = 0; i < board_count; i++)
  {
    if (WindowShouldClose())
      return EXIT_FAILURE;
    BeginDrawing();
      ClearBackground(CLAY_COLOR_TO_RAYLIB_COLOR(theme::background_color));
      DrawRectangleLinesEx(
        {
          XMAX/2.f - LOADING_W/2.f, YMAX/2.f - LOADING_H/2.f,
          LOADING_W, LOADING_H
        },
        5.f,
        GRAY
      );
      DrawRectangleRec(
        {
          XMAX/2.f - LOADING_W/2.f, YMAX/2.f - LOADING_H/2.f,
          LOADING_W*float(i)/board_count, LOADING_H
        },
        SKYBLUE
      );
      spritesheets.push(Texture{});
      TextureDumpLoad(spritesheets[-1], s);
    EndDrawing();
  }
  return EXIT_SUCCESS;
}


void destroy_res()
{
  UnloadFont(theme::fonts[0]);
  const isize l = spritesheets.len();
  for (isize i = 0; i < l; i++)
  {
    UnloadTexture(spritesheets[i]);
  }
  if (source_cobz)
  {
    fclose(source_cobz);
  }
  spritesheets.destroy();
  texs.destroy();
  boards.destroy();
}

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

list<Texture> spritesheets;
list<TexInfo> texs;
list<Board> boards;
Clay_TextElementConfig font;
Texture btns[5];
FILE* source_cobz;
#ifdef DEBUG
#include <dlfcn.h>
void(*layout_home)(Clay_RenderCommandArray& array);
void(*layout_options)(Clay_RenderCommandArray& array);
void load_layouts()
{
  void* h = dlopen("cxml/ui.so", RTLD_LAZY);
  assert(h != 0);
  layout_home = decltype(layout_home)(dlsym(h, "layout_home"));
  layout_options = decltype(layout_options)(dlsym(h, "layout_options"));
  assert(layout_home != 0);
  assert(layout_options != 0);
  dlclose(h);
}
#endif

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
  s.read(buf, 4);
  // Here is some weird shenanigans because libspng doesn't seem to read the
  // whole image data and stops before the end. Thus these alignement anchors
  // are necessary (the alignement anchor being "IMG\x00")
  while (memcmp(buf, "IMG", 4) != 0)
  {
    buf[0] = buf[1];
    buf[1] = buf[2];
    buf[2] = buf[3];
    buf[3] = fgetc(s._f);
    if (buf[3] == EOF)
    {
      TraceLog(
        LOG_ERROR,
        "Image misalignement. ",
        buf[0], buf[1], buf[2], buf[3]
      );
      goto error;
    }
  }

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



int init_res(Ref<Stream> s)
{
  assert(sizeof(isize)==8);
  isize board_count, tex_count;

  source_cobz = s._f; // global set

  current_actions.init();

  // NOTE: Font is loaded with init_theme
  theme::text_space_width = MeasureTextEx(
    theme::fonts[0],
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

  // Rectangles
  tex_count = s.read<isize>();
  texs.prealloc(tex_count);
  fread(texs.data(), sizeof(TexInfo), tex_count, s._f);
  texs.set_len(tex_count);
  // Spritesheets
  tex_count = s.read<isize>();
  spritesheets.prealloc(tex_count);
  for (isize i = 0; i < tex_count; i++)
  {
    spritesheets.push(Texture{});
    TextureDumpLoad(spritesheets[-1], s);
  }
  spritesheets.set_len(tex_count);
    
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
  spritesheets.destroy();
  texs.destroy();
  boards.destroy();
}

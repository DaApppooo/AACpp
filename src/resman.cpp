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

Texture ssld; // loaded spritesheet
list<long> ssfl; // spritesheet file locations
list<TexRect> rects;
list<Board> boards;
Clay_TextElementConfig font;
extern "C"
{ Texture btns[5]; }
FILE* source_cobz;

View<FixedString> current_actions;

void TexRect::draw(Rectangle target)
{
  DrawTexturePro(
    ssld,
    rect, target,
    {0.f, 0.f}, 0.f, WHITE
  );
}

void tex_hold(int ssid)
{
  assert(ssld.id == 0);
  fseek(source_cobz, ssfl[ssid], SEEK_SET);
  TextureDumpLoad(ssld, {source_cobz});
}
void tex_drop(int ssid)
{
  UnloadTexture(ssld);
  ssld.id = 0;
}

void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
        abort();
    }
}

#ifdef _DEBUG
    #define GL_CHECK(stmt) do { \
            stmt; \
            CheckOpenGLError(#stmt, __FILE__, __LINE__); \
        } while (0)
#else
    #define GL_CHECK(stmt) stmt
#endif

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
  fseek(s._f, sizeof(int), SEEK_CUR); // skip board id

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
  
  GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
  GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
  GL_CHECK(glGenTextures(1, &id));
  GL_CHECK(glBindTexture(GL_TEXTURE_2D, id));
  GL_CHECK(glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGBA,
    ihdr.width,
    ihdr.height,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    data
  ));
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  LINUX(
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE));
  )
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
  GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

  tex.id = id;
  tex.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
  tex.height = ihdr.height;
  tex.width  = ihdr.width;
  tex.mipmaps = 1;
  
  if (data != nullptr)
    free(data);
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
  ssld.id = 0;
  rects.init();
  boards.init();
  current_actions.init();
  source_cobz = nullptr;

  btns[BTI_OPT] = LoadTexture("assets/opt.png");
  btns[BTI_BACKSPACE] = LoadTexture("assets/kb.png");
  btns[BTI_CLEAR] = LoadTexture("assets/cl.png");
  btns[BTI_PLAY] = LoadTexture("assets/pa.png");
  btns[BTI_UP] = LoadTexture("assets/au.png");
}
void reset_res()
{
  UnloadTexture(ssld);
  ssld.id = 0;
  rects.clear();
  boards.clear();
  current_actions.init();
}

int res_load_boardset(Ref<Stream> s)
{
  static_assert(sizeof(isize)==8);
  isize board_count, tex_count;
  source_cobz = s._f; // global set
  s >> (isize&) board_count;
  boards.prealloc(board_count);
  for (isize i = 0; i < board_count; i++)
  {
    Board b;
    b.deserialize(s);
    boards.push(b);
  }

  s >> (isize&) tex_count; // rectangles
  s >> (isize&) board_count; // spritesheets
  rects.prealloc(tex_count);
  fread(rects.data(), sizeof(TexRect), tex_count, s._f);
  rects.set_len(tex_count);
  // Spritesheets
  const float LOADING_W = 0.1f * XMAX;
  const float LOADING_H = 0.05f * YMAX;
  assert(board_count != 0);
  ssfl.prealloc(board_count);
  int board_id = -1;
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
      s.align_until_anchor("IMG");
      s >> (int&) board_id;
      boards[board_id].ssid = i;
      // save before anchor and board id
      ssfl.push(ftell(source_cobz) - 4 - sizeof(int));
    EndDrawing();
  }
  tex_hold(boards[0].ssid);
  return EXIT_SUCCESS;
}


void destroy_res()
{
  UnloadFont(theme::fonts[0]);
  UnloadTexture(ssld);
  if (source_cobz)
    fclose(source_cobz);
  ssfl.destroy();
  rects.destroy();
  boards.destroy();
}

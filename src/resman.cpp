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

constexpr int THREAD_TEXLOADER_LIFE_BIT = 0b01;
constexpr int THREAD_TEXLOADER_CH_BIT   = 0b10;
Thread thread_texloader;
int thread_texloader_status;
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

// Just moves the file pointer forward
void TextureFakeLoad(Stream s)
{
  static char buf[3];
  int channels;
  Image temp;
  s.read(buf, 3);
  assert(memcmp(buf, "IMG", 3) == 0); // if this fails, you're not reading a texture
  // stbi doesn't implement this wihout memory allocation
  // so we have to actually do this even though it's useless
  void* p = stbi_load_from_file(s._f, &temp.width, &temp.height, &channels, 0);
  free(p);
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

void* thread_texloader_func(void*)
{
  Stream s = {source_cobz};
  for (isize i = 0; i < texs.len(); i++)
  {
    texs[i].seek = ftell(s._f);
    TextureFakeLoad(s);
    if (i % 256 == 0)
      TraceLog(LOG_INFO, "TEXLOADER: Loading offsets (%i/%i)", i, texs.len());
  }
  TraceLog(LOG_INFO, "TEXLOADER: Loaded offsets !");
  while (thread_texloader_status & THREAD_TEXLOADER_LIFE_BIT)
  {
    // add a little of process sleep to keep the cpu usage of this process low.
    WaitTime(0.3);
    for (int i = 0; i < texs.len(); i++)
    {
      if (texs[i].ideal_state && !texs[i].loaded)
      {
        fseek(source_cobz, texs[i].seek, SEEK_SET);
        TextureDumpLoad(texs[i].ioload, {source_cobz});
        texs[i].loaded = 1;
      }
      else if (!texs[i].ideal_state && texs[i].loaded)
      {
        UnloadTexture(texs[i].tex);
        texs[i].loaded = 0;
      }
    }
  }
  for (int i = 0; i < texs.len(); i++)
  {
    if (texs[i].ioload.data != nullptr)
    {
      UnloadImage(texs[i].ioload);
      texs[i].ioload.data = nullptr;
    }
    if (texs[i].loaded)
    {
      UnloadTexture(texs[i].tex);
      texs[i].loaded = 0;
    }
  }
  return nullptr;
}

void hold_tex(int valid_tex_id)
{
  texs[valid_tex_id].ideal_state = 1;
}

void drop_tex(int valid_tex_id)
{
  texs[valid_tex_id].ideal_state = 0;
}

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
  for (isize i = 0; i < tex_count; i++)
  {
    // Texture tex;
    // TextureDumpLoad(tex, s);
    Texture tex;
    tex.id = 0;
    TextureCargo cargo;
    cargo.loaded = 0;
    cargo.ideal_state = 0;
    // cargo.seek = ftell(s._f); Moved in proc_texloader_func
    cargo.tex = tex;
    // TextureFakeLoad(s); Same ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    texs.push(cargo);
  }

  thread_texloader_status = THREAD_TEXLOADER_LIFE_BIT;
  thread_texloader.launch(&thread_texloader_func, nullptr);
  
  return EXIT_SUCCESS;
}

void destroy_res()
{
  UnloadFont(Raylib_fonts[0].font);
  for (int i = 0; i < texs.len(); i++) {
    drop_tex(i);
  }
  thread_texloader_status &= ~THREAD_TEXLOADER_LIFE_BIT; // kill
  thread_texloader.join();
  texs.destroy();
  boards.destroy();
}

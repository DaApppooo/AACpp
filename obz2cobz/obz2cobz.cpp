#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <raylib.h>
#include "../include/plutosvg.h"
#include "../include/list.hpp"
const char* help =
"Usage:\n"
"obz2cobz.py <src>.obz <dst>.cobz\n"
"\n"
"Descr:\n"
"Compiles Open Board Zip files (exported from Board Builder),\n"
"into a Compiled Open Board Zip for this custom AAC program I wrote (AACpp).\n"
"\n"
"This is intended to be nothing more than a prototype. A full C++ (probably)\n"
"version will be written in the future. It's just that zip+json is such a "
"hassle\n"
"to parse in C.\n";


constexpr int SUPPORTED_IMAGE_EXT_COUNT = 11;
const char* const SUPPORTED_IMAGE_EXT[SUPPORTED_IMAGE_EXT_COUNT] = {
  "png","jpg","jpeg","tga","bmp","psd","gif","hdr","pic","pnm","svg"
};
bool is_image_ext_supported(const char* extension)
{
  for (int i = 0; i < SUPPORTED_IMAGE_EXT_COUNT; i++)
    if (str_eq(extension, SUPPORTED_IMAGE_EXT[i]))
      return true;
  return false;
}
constexpr int LEGACY_COLORS_COUNT = 148;
const char* const LEGACY_COLOR_NAMES[LEGACY_COLORS_COUNT] = {
"aliceblue",
"antiquewhite",
"aqua",
"aquamarine",
"azure",
"beige",
"bisque",
"black",
"blanchedalmond",
"blue",
"blueviolet",
"brown",
"burlywood",
"cadetblue",
"chartreuse",
"chocolate",
"coral",
"cornflowerblue",
"cornsilk",
"crimson",
"cyan",
"darkblue",
"darkcyan",
"darkgoldenrod",
"darkgray",
"darkgrey",
"darkgreen",
"darkkhaki",
"darkmagenta",
"darkolivegreen",
"darkorange",
"darkorchid",
"darkred",
"darksalmon",
"darkseagreen",
"darkslateblue",
"darkslategray",
"darkslategrey",
"darkturquoise",
"darkviolet",
"deeppink",
"deepskyblue",
"dimgray",
"dimgrey",
"dodgerblue",
"firebrick",
"floralwhite",
"forestgreen",
"fuchsia",
"gainsboro",
"ghostwhite",
"gold",
"goldenrod",
"gray",
"grey",
"green",
"greenyellow",
"honeydew",
"hotpink",
"indianred",
"indigo",
"ivory",
"khaki",
"lavender",
"lavenderblush",
"lawngreen",
"lemonchiffon",
"lightblue",
"lightcoral",
"lightcyan",
"lightgoldenrodyellow",
"lightgray",
"lightgrey",
"lightgreen",
"lightpink",
"lightsalmon",
"lightseagreen",
"lightskyblue",
"lightslategray",
"lightslategrey",
"lightsteelblue",
"lightyellow",
"lime",
"limegreen",
"linen",
"magenta",
"maroon",
"mediumaquamarine",
"mediumblue",
"mediumorchid",
"mediumpurple",
"mediumseagreen",
"mediumslateblue",
"mediumspringgreen",
"mediumturquoise",
"mediumvioletred",
"midnightblue",
"mintcream",
"mistyrose",
"moccasin",
"navajowhite",
"navy",
"oldlace",
"olive",
"olivedrab",
"orange",
"orangered",
"orchid",
"palegoldenrod",
"palegreen",
"paleturquoise",
"palevioletred",
"papayawhip",
"peachpuff",
"peru",
"pink",
"plum",
"powderblue",
"purple",
"rebeccapurple",
"red",
"rosybrown",
"royalblue",
"saddlebrown",
"salmon",
"sandybrown",
"seagreen",
"seashell",
"sienna",
"silver",
"skyblue",
"slateblue",
"slategray",
"slategrey",
"snow",
"springgreen",
"steelblue",
"tan",
"teal",
"thistle",
"tomato",
"turquoise",
"violet",
"wheat",
"white",
"whitesmoke",
"yellow",
"yellowgreen"
};
const u32 LEGACY_COLOR_BYTES[LEGACY_COLORS_COUNT] = {
0xF0F8FFFF,
0xFAEBD7FF,
0x00FFFFFF,
0x7FFFD4FF,
0xF0FFFFFF,
0xF5F5DCFF,
0xFFE4C4FF,
0x000000FF,
0xFFEBCDFF,
0x0000FFFF,
0x8A2BE2FF,
0xA52A2AFF,
0xDEB887FF,
0x5F9EA0FF,
0x7FFF00FF,
0xD2691EFF,
0xFF7F50FF,
0x6495EDFF,
0xFFF8DCFF,
0xDC143CFF,
0x00FFFFFF,
0x00008BFF,
0x008B8BFF,
0xB8860BFF,
0xA9A9A9FF,
0xA9A9A9FF,
0x006400FF,
0xBDB76BFF,
0x8B008BFF,
0x556B2FFF,
0xFF8C00FF,
0x9932CCFF,
0x8B0000FF,
0xE9967AFF,
0x8FBC8FFF,
0x483D8BFF,
0x2F4F4FFF,
0x2F4F4FFF,
0x00CED1FF,
0x9400D3FF,
0xFF1493FF,
0x00BFFFFF,
0x696969FF,
0x696969FF,
0x1E90FFFF,
0xB22222FF,
0xFFFAF0FF,
0x228B22FF,
0xFF00FFFF,
0xDCDCDCFF,
0xF8F8FFFF,
0xFFD700FF,
0xDAA520FF,
0x808080FF,
0x808080FF,
0x008000FF,
0xADFF2FFF,
0xF0FFF0FF,
0xFF69B4FF,
0xCD5C5CFF,
0x4B0082FF,
0xFFFFF0FF,
0xF0E68CFF,
0xE6E6FAFF,
0xFFF0F5FF,
0x7CFC00FF,
0xFFFACDFF,
0xADD8E6FF,
0xF08080FF,
0xE0FFFFFF,
0xFAFAD2FF,
0xD3D3D3FF,
0xD3D3D3FF,
0x90EE90FF,
0xFFB6C1FF,
0xFFA07AFF,
0x20B2AAFF,
0x87CEFAFF,
0x778899FF,
0x778899FF,
0xB0C4DEFF,
0xFFFFE0FF,
0x00FF00FF,
0x32CD32FF,
0xFAF0E6FF,
0xFF00FFFF,
0x800000FF,
0x66CDAAFF,
0x0000CDFF,
0xBA55D3FF,
0x9370DBFF,
0x3CB371FF,
0x7B68EEFF,
0x00FA9AFF,
0x48D1CCFF,
0xC71585FF,
0x191970FF,
0xF5FFFAFF,
0xFFE4E1FF,
0xFFE4B5FF,
0xFFDEADFF,
0x000080FF,
0xFDF5E6FF,
0x808000FF,
0x6B8E23FF,
0xFFA500FF,
0xFF4500FF,
0xDA70D6FF,
0xEEE8AAFF,
0x98FB98FF,
0xAFEEEEFF,
0xDB7093FF,
0xFFEFD5FF,
0xFFDAB9FF,
0xCD853FFF,
0xFFC0CBFF,
0xDDA0DDFF,
0xB0E0E6FF,
0x800080FF,
0x663399FF,
0xFF0000FF,
0xBC8F8FFF,
0x4169E1FF,
0x8B4513FF,
0xFA8072FF,
0xF4A460FF,
0x2E8B57FF,
0xFFF5EEFF,
0xA0522DFF,
0xC0C0C0FF,
0x87CEEBFF,
0x6A5ACDFF,
0x708090FF,
0x708090FF,
0xFFFAFAFF,
0x00FF7FFF,
0x4682B4FF,
0xD2B48CFF,
0x008080FF,
0xD8BFD8FF,
0xFF6347FF,
0x40E0D0FF,
0xEE82EEFF,
0xF5DEB3FF,
0xFFFFFFFF,
0xF5F5F5FF,
0xFFFF00FF,
0x9ACD32FF
};
// return -1 if color wasn't found, otherwise returns a u32 (within a i64) of the color as rgba8 (r is big end)
i64 u32_legacy_color(const char* name)
{
  int begin = 0, end = LEGACY_COLORS_COUNT;
  int mid;
  const int len = str_len(name)+1;
  char* s = (char*)alloca(len);
  for (int i = 0; i < len; i++)
  {
    if (name[i] >= 'A' && name[i] <= 'Z')
      s[i] = name[i] - 'A' + 'a';
    else
      s[i] = name[i];
  }
  while (begin != end)
  {
    mid = (begin + end) / 2;
    const int cmp = strcmp(s, LEGACY_COLOR_NAMES[mid]);
    if (cmp == -1)
      begin = mid;
    else if (cmp == 1)
      end = mid;
    else
      return LEGACY_COLOR_BYTES[mid];
  }
  return -1;
}

void svg2png(ByteBuffer in, ByteBuffer& out, const char* const info)
{
  out = ByteBuffer::init();
  const plutosvg_document_t* doc = plutosvg_document_load_from_data(
    (i8*)in.data, in.len, 128, 128, nullptr, nullptr
  );
  if (doc == nullptr)
  { printf("ERROR: [%s] Failed to read svg document.", info); return; }
  const plutovg_surface_t* surf = plutosvg_document_render_to_surface(
    doc, nullptr, 128, 128, nullptr, nullptr, nullptr
  );
  struct Pack { ByteBuffer& out; u64 cap; }
    pack = { out, 0 };
  out.dyn_begin(pack.cap);
  plutovg_surface_write_to_png_stream(
    surf,
    [](void* _pack, void* data, int size)
    {
      auto pack = (Pack*)_pack;
      pack->out.dyn_push(pack->cap, ByteBuffer::ref((u8*)data, size));
    },
    &pack
  );
  out.dyn_end(pack.cap);
  return;
}

void expect(bool cond, const char* const err_msg)
{
  if (!cond)
  {
    dblog(LOG_FATAL, "Expect failed: %s", err_msg);
    abort();
  }
}
void want(bool cond, const char* const warn_msg)
{
  if (!cond)
    dblog(LOG_WARNING, "%s", warn_msg);
}

struct ivec2 { int x,y; };

struct Cell;
struct Board;
struct ImageData
{
  static ImageData init();
  int width();
  int height();
  void down_scale_pow2(int downscale_factor);
  void paste(ImageData& img, int x, int y);
};
struct Rect
{
  float x,y,w,h;
  int spritesheet_id;
  bool locked;
  void serialize(Stream s)
  { s << spritesheet_id << x << y << w << h; }
};

struct Obj
{
  string obz_tex_id;
  string obz_board_id;
  ImageData img;
  Rect rect;
};
struct Fit
{
  Rect rect;
  int obj_idx;
  bool could_contain(Rect rsmol)
  { return rect.w >= rsmol.w && rect.h >= rsmol.h; }
};
struct CompiledOBZ
{
  list<Obj> textures;
  list<Board> boards;
  ivec2 gen_spritesheet_precursors(list<Obj*>& objs)
  {
    int minw = 0;
    int maxw = 0;
    for (int i = 0; i < objs.len(); i++)
    {
      int downscale_factor = 0;
      objs[i]->rect.w = objs[i]->img.width();
      objs[i]->rect.h = objs[i]->img.height();
      while (objs[i]->rect.w > 300)
      {
        objs[i]->rect.w = floorf(objs[i]->rect.w / 2);
        objs[i]->rect.h = floorf(objs[i]->rect.h / 2);
        downscale_factor += 2;
      }
      if (objs[i]->rect.w > maxw)
        maxw = objs[i]->rect.w;
      objs[i]->img.down_scale_pow2(downscale_factor);
    }
    maxw *= 2;
    // yeah... im lazy...
    // but who wants to write a sorting algorithm in big 2025 anyway ???
    std::sort(objs.data(), objs.data()+(objs.len()+1), [](Obj* a, Obj* b){
      if (a->rect.w == a->rect.h)
        return a->rect.h < b->rect.h;
      return a->rect.w < b->rect.w;
    });
    minw = objs[-1]->rect.w;
    // Set y positions accordingly
    float y = 0;
    for (int i = 0; i < objs.len(); i++)
    {
      objs[i]->rect.x = 0;
      objs[i]->rect.y = y;
      y += objs[i]->rect.h;
    }
    list<Fit> fits;
    fits.init();
    fits.prealloc(objs.len());
    ivec2 ssdims = {0,0}; // spritesheet dimensions
    int base_fixed = 0;
    for (int i = 1; i < objs.len(); i++)
    {
      for (int j = base_fixed; j < i; j++)
      {
        if (fits[j].could_contain(objs[i]->rect))
        {
          objs[i]->rect.x = fits[j].rect.x;
          objs[i]->rect.y = fits[j].rect.y;
          fits.insert(Fit{
            {
              objs[i]->rect.x + objs[i]->rect.w,
              objs[i]->rect.y,
              maxw - (objs[i]->rect.x + objs[i]->rect.w),
              objs[i]->rect.h
            },
            i
          },j);
          fits[j+1].rect.y += objs[i]->rect.h;
          fits[j+1].rect.h -= objs[i]->rect.h;
          int fi = j+2;
          for (; fi < fits.len(); fi++)
          {
            if (fits[fi].obj_idx == i)
            {
              fits.rmv(fi);
              break;
            }
          }
          for (; fi < fits.len(); fi++)
            fits[fi].rect.y -= objs[i]->rect.h;
          for (int k = i+1; k < objs.len(); k++)
            objs[k]->rect.y -= objs[i]->rect.h;
          break;
        }
      }
    }
    ssdims.x = maxw;
    for (int i = 0; i < objs.len(); i++)
    {
      const float Y = objs[i]->rect.y+objs[i]->rect.h;
      if (Y > ssdims.y)
      {
        ssdims.y = Y;
      }
    }
    return ssdims;
  }

  ImageData gen_one_spritesheet(list<Obj*>& objs, ivec2 dims, int spritesheet_id)
  {
    auto spritesheet = ImageData::init();
    for (int i = 0; i < objs.len(); i++)
    {
      assert(objs[i]->rect.spritesheet_id == -1);
      objs[i]->rect.spritesheet_id = spritesheet_id;
      spritesheet.paste(objs[i]->img, objs[i]->rect.x, objs[i]->rect.y);
    }
    return spritesheet;
  }

  list<ImageData> gen_all_spritesheet();
};

struct Board
{
  int w, h;
  int parent_idx;
  list<Cell> cells;
  string name;
  string obz_id;
  static inline Board init() {
    Board r = { 0, 0, -1, {}, {}, {} };
    r.cells.init();
    r.name.init();
    r.obz_id.init();
    return r;
  }
  void serialize(Stream s);
};
struct Cell
{
  string name;
  int tex_id;
  int child;
  u32 background, border; // colors
  list<string> actions;
  string obz_child_id;
  string obz_tex_id;
  string obz_id;
  ivec2 obz_xy;
  void set_child_obz(CompiledOBZ& cobz, string& obz_id)
  { obz_child_id.hold(obz_id); }
  void set_child_idx(CompiledOBZ& cobz, int idx)
  {
    assert(obz_child_id.len() != 0);
    child = idx;
    if (cobz.boards[idx].parent_idx == -1)
    {
      cobz.boards[idx].parent_idx = idx;
    }
    else
      assert(cobz.boards[idx].parent_idx == idx);
  }
  void serialize(Stream s)
  {
    i64 len;
    s.write_anchor("CLL");
    s << (i32&) tex_id;
    len = name.len();
    fwrite(&len, sizeof(len), 1, s._f);
    fwrite(name.data(), len, 1, s._f);
    len = actions.len();
    fwrite(&len, sizeof(len), 1, s._f);
    for (i64 i = 0; i < len; i++)
    {
      i64 lena = actions[i].len();
      fwrite(&lena, sizeof(lena), 1, s._f);
      fwrite(actions[i].data(), lena, 1, s._f);
    }
    s << (i32&) child;
    s << (u32&) background << (u32&) border;
  }
};

void Board::serialize(Stream s)
{
  const int cell_count = cells.len();
  s << w << h << parent_idx << cell_count;
  for (int i = 0; i < cell_count; i++)
  {
    cells[i].serialize(s);
  }
}

list<ImageData> CompiledOBZ::gen_all_spritesheet()
{
  list<ImageData> spritesheets;
  list<Obj*> objs;
  ivec2 ssdims;
  objs.init();
  spritesheets.init();
  for (int i = 0; i < boards.len(); i++)
  {
    objs.clear();
    for (int j = 0; j < textures.len(); j++)
      if (textures[j].obz_board_id == boards[i].obz_id)
        objs.push(&textures[j]);
    ssdims = gen_spritesheet_precursors(objs);
    spritesheets.push(gen_one_spritesheet(objs, ssdims, i));
  }
  return spritesheets;
}

float depercent(char* expr, float max)
{
  const int len = str_len(expr);
  if (expr[len-1] == '%')
  {
    expr[len-1] = 0;
    const float r = strtof(expr, nullptr)/100.f * max;
    expr[len-1] = '%';
    return r;
  }
  else
    return strtof(expr.data(), nullptr);
}
u32 hexchar(char c)
{
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= '0' && c <= '9') return c - '0';
  dblog(LOG_ERROR, "Invalid hexadecimal value: %c", c);
  return 0;
}
u32 rgba(u32 r, u32 g, u32 b, u32 a) { return (r << 24) | (g << 16) | (b << 8) | a; }
u32 parse_color(string expr, u32 default_)
{
#define FIND_NEXT_CHAR(IDX, CHAR) \
    while (expr[IDX] != CHAR && expr[IDX] != 0) IDX++; \
    if (expr[IDX] == 0) \
    { dblog(LOG_WARNING, "Inavlid rgb color format."); return default_; }
#define READ_COLOR_COMP(COMP, NEXT_END) \
    FIND_NEXT_CHAR(comma, NEXT_END); \
    expr[comma] = 0; \
    COMP = depercent(expr.data()+comma, 255); \
    comma++;

  const int len = expr.len();
  if (len == 0)
    return default_;
  if (str_startswith(expr.data(), "rgb(") && str_endswith(expr.data(), ")"))
  {
    int comma = 4;
    u32 r,g,b;
    READ_COLOR_COMP(r, ',');
    READ_COLOR_COMP(g, ',');
    READ_COLOR_COMP(b, ')');
    return rgba(r,g,b,255);
  }
  else if (
    str_startswith(expr.data(), "rgba(") && str_endswith(expr.data(), ")")
  ) {
    int comma = 5;
    u32 r,g,b,a;
    READ_COLOR_COMP(r, ',');
    READ_COLOR_COMP(g, ',');
    READ_COLOR_COMP(b, ',');
    READ_COLOR_COMP(a, ')');
    return rgba(r,g,b,a);
  }
  else if (expr[0] == '#')
  {
    if (len == 7)
    {
      u32 ret = 0;
      for (int i = 0; i < 6; i++)
      { ret |= hexchar(expr[i+1]) << (i*4 + 8); }
      return ret;
    }
    else if (len == 9)
    {
      u32 ret = 0;
      for (int i = 0; i < 8; i++)
      { ret |= hexchar(expr[i+1]) << (i*4); }
      return ret;
    }
    else
    {
      dblog(
        LOG_WARNING,
        "Expected 6 or 8 digit hex color format, but got '%s'",
        expr.data()
      );
      return default_;
    }
  }
  else
  {
    const i64 legacy_color = u32_legacy_color(expr.data());
    if (legacy_color != -1)
      return legacy_color;
    dblog(LOG_WARNING, "Unknown color format: '%s'", expr.data());
    return default_;
  }
}

int main()
{
  puts("Hello");
  return 0;
}

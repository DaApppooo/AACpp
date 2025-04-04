#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <librsvg/rsvg.h>
#include <cairo/cairo.h>
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

using u64 = uint64_t;
using u32 = uint32_t;
using u8 = uint8_t;

// A half dynamic heap buffer
struct ByteBuffer
{
  u8* data;
  u64 len : 63;
  bool owned : 1;
  static inline ByteBuffer init() { return {nullptr, 0, 0}; }
  static inline ByteBuffer hold(u8* src, u32 size) { return {src, size, 1}; }
  static inline ByteBuffer ref(u8* src, u32 size) { return {src, size, 0}; }
  void alloc(int size)
  {
    if (data == nullptr)
      data = (u8*)malloc(size);
    else
      data = (u8*)realloc(data, size);
    len = size;
  }
  void dyn_begin(u64& out_cap)
  {
    out_cap = len;
  }
  void dyn_push(u64& io_cap, const ByteBuffer src)
  {
    len += src.len;
    if (len > io_cap)
    {
      do
        io_cap = io_cap + io_cap/2 + 1;
      while (len > io_cap);
      if (data == nullptr)
        data = (u8*)malloc(io_cap);
      else
        data = (u8*)realloc(data, io_cap);
    }
    memcpy(data+len-src.len, src.data, src.len);
  }
  void dyn_end(u64& io_cap)
  {  }
  void destroy()
  { if (data) { free(data); data = nullptr; } }
};

void svg2png(ByteBuffer in, ByteBuffer& out)
{
  GError* error = nullptr;
  RsvgHandle* handle = nullptr;
  cairo_t* ctx = nullptr;
  cairo_surface_t* surf = nullptr;
  surf = cairo_image_surface_create(
    CAIRO_FORMAT_ARGB32, // why not do rgba32 like everyone ???
                         // you're not "special" for it bitch !
                         // just annoying...
    128,
    128
  );
  ctx = cairo_create(surf);
  handle = rsvg_handle_new_from_data(in.data, in.len, &error);
  if (error != nullptr)
    goto ERROR;
  const bool res = rsvg_handle_render_document (
    handle,
    ctx,
    RsvgRectangle{0, 0, 128, 128},
    &error
  );
  if (error != nullptr)
    goto ERROR;
  return;
ERROR:
  printf("ERROR: (svg2png) %s", error->message);
  g_error_free(error);
  if (handle != nullptr)
    g_object_unref(handle);
  if (surf != nullptr)
    cairo_surface_destroy(surf);
  if (ctx != nullptr)
    cairo_destroy(ctx);
  out = ByteBuffer::init();
  return;
}

int main()
{
  
}

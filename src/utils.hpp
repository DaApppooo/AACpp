#pragma once
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <raylib.h>
#include <cstdio>
#ifndef H_UTILS
#define H_UTILS

#define todo() dblog(LOG_FATAL, "TODO REACHED %s:%i", __FILE__, __LINE__); abort()

using u8 = uint8_t;
using u64 = uint64_t;
using i64 = int64_t;

inline void memcpy_lin(
  void* const dst, int dst_a,
  const void* const src, int src_a,
  int count
) {
  u8 *dst_ = (u8*)dst,
     *src_ = (u8*)src;
  for (int i = 0; i < count; i++)
  {
    int b;
    for (b = 0; b < std::min(dst_a, src_a); b++)
    {
      *dst_ = *src_;
    }
    for (; b < std::max(dst_a, src_a); b++)
    {
      *dst_ = 0;
    }
    dst_ += dst_a;
    src_ += src_a;
  }
}

struct FixedString
{
  using Char = int;
  u64 _len;
  Char* _data;
  inline i64 len() const
  {
    return _len;
  }
  inline i64 mem_size() const
  {
    return sizeof(Char)*_len;
  }
  inline void deserialize(FILE* f)
  {
    fread(&_len, sizeof(u64), 1, f);
    assert(_len >= 0);
    if (_len == 0)
    {
      _data = nullptr;
      return;
    }
    _data = (Char*)malloc(sizeof(Char)*_len);
    fread(_data, _len, 1, f);
  }
  inline void to_ascii_buffer(char* buff, u64 max_len, char err)
  {
    int i;
    for (i = 0; i < std::min(max_len-1, _len); i++)
    {
      if (isprint(_data[i]))
      {
        buff[i] = _data[i];
      }
      else
      {
        buff[i] = err;
      }
    }
    buff[i] = 0;
  }
  inline int& operator [] (int i)
  {
    return _data[i];
  }
  void destroy()
  {
    if (_data != nullptr)
    {
      free(_data);
      _data = nullptr;
      _len = 0;
    }
  }
};

long FileEditTime(const char* filename);

const char* open_file_dialogue();
void init_tts();
void tts_push(FixedString& w);
void tts_backspace(); // remove last pushed piece or word
void tts_clear();
const char* tts_fill_final_buffer();
void tts_play();
void destroy_tts();

const char* fix2var_utf8(const FixedString& s, char* ob);

inline constexpr int str_len(const char* s)
{
  if (s == nullptr) return 0;
  if (*s == 0) return 0;
  int c = 1;
  while (*(++s) != 0) c++;
  return c;
}

inline constexpr int len(const char* s)
{
  return str_len(s);
}

template <class T> using Own = T;
template <class T> using Ref = T;

struct Stream
{
  FILE* _f;
  template <class T>
  inline T read()
  { T ret; fread(&ret, sizeof(T), 1, _f); return ret; }
  inline void read(char* out_s, int len)
  {
    fread(out_s, len, 1, _f);
  }

  template <class T>
  inline void write(T o)
  { fwrite(&o, sizeof(T), 1, _f); }
  inline void write(const char* s, int len)
  { fwrite(s, len, 1, _f); }
};

using fvec2 = Vector2;

extern bool edit_mode;
extern float XMAX, YMAX, dt;
extern bool edit_mode;

namespace ctrl
{
  extern fvec2 mpos;
  extern bool mouse_down;
  extern char read;
}

#endif

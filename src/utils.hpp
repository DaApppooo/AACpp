#pragma once
#include "clay.h"
#include <cstdlib>
#include <cstring>
#include <raylib.h>
#include <cstdio>
#ifndef H_UTILS
#define H_UTILS

#define todo() dblog(LOG_FATAL, "TODO REACHED %s:%i", __FILE__, __LINE__); abort()

const char* open_file_dialogue();
void init_tts();
void tts_push_word(const char* w); // adds a space after
void tts_push_piece(const char* p); // doesn't add a space
void tts_backspace(); // remove last pushed piece or word
void tts_clear();
const char* tts_fill_final_buffer();
void destroy_tts();

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
extern float XMAX, YMAX;
extern Clay_TextElementConfig font;
extern bool edit_mode;

namespace ctrl
{
  extern fvec2 mpos;
  extern bool mouse_down;
  extern char read;
}

#endif

#include "utils.hpp"
#include "nfd.h"
#include <filesystem>

float XMAX, YMAX;

long FileEditTime(const char* filename)
{
  // i swear, the globglobgabgalab is one of the prettiest princess in
  //  comparison to c++
  const auto ft = 
    std::filesystem::last_write_time(filename);
  return ft.time_since_epoch().count();
  // here we have no idea what unit is used bcz ofc it's not even specified in
  //  the doc...
}

namespace ctrl
{
  fvec2 mpos;
  bool mouse_down;
}

#if 0 && PLATFORM == PLATFORM_ANDROID
#error THIS PART IS TODO
void BringKeyboard()
{
  todo();
}
void init_tts()
{
  todo();
}
inline void BringKeyboard()
{}
#endif

#ifdef __linux__
# ifndef MAX_PATH
#  define MAX_PATH 1024
# endif

char _PATH_BUFF[MAX_PATH] = { 0 };
const char* open_file_dialogue()
{
  nfdchar_t* opath;
  nfdresult_t r = NFD_OpenDialog(nullptr, nullptr, &opath);
  if (r == NFD_OKAY)
  {
    const int len = str_len(opath);
    memcpy(_PATH_BUFF, opath, (len+1)*sizeof(char));
    return _PATH_BUFF;
  }
  else if (r == NFD_CANCEL)
  {
    return 0;
  }
  else
  {
    TraceLog(LOG_ERROR, "[NFD] Error opening file: %s", NFD_GetError());
    return 0;
  }
}

constexpr int TTS_MSG_LEN = 2048;
constexpr int TTS_CLI_LEN = 1024;
char TTS_MSG_BUILDER[TTS_MSG_LEN];
int TTS_POS = 0;

char TTS_MSG_FINAL[TTS_MSG_LEN];
char TTS_CLI[TTS_CLI_LEN];
void init_tts()
{
  memset(TTS_MSG_BUILDER, 0, sizeof(char)*2048);
  memset(TTS_CLI, 0, sizeof(char)*1024);
  const char* p = secure_getenv("TTS");
  if (p == nullptr)
  {
    TraceLog(LOG_ERROR, "\x1b[31m[TTS] No TTS environement variable found !\x1b[0m");
  }
  else
  {
    strcpy(TTS_CLI, p);
    TraceLog(LOG_INFO, "[TTS] Will use '%s' as input popened process for tts.", TTS_CLI);
  }
}
void tts_push_word(const char* w)
{
  const int len = str_len(w);
  strncpy(TTS_MSG_BUILDER + TTS_POS, w, TTS_MSG_LEN - TTS_POS - 1);
  TTS_POS += len;
  TTS_MSG_BUILDER[len] = '\x01';
  TTS_POS++;
}
void tts_push_piece(const char* p)
{
  const int len = str_len(p);
  strncpy(TTS_MSG_BUILDER + TTS_POS, p, TTS_MSG_LEN - TTS_POS - 1);
  TTS_POS += len;
  TTS_MSG_BUILDER[len] = '\x02';
  TTS_POS++;
}
void tts_backspace()
{
  while (
     TTS_POS > 0
  && ! (
       TTS_MSG_BUILDER[TTS_POS] == '\x01'
    || TTS_MSG_BUILDER[TTS_POS] == '\x02'
       )
  ) {
      TTS_POS--;
  }
}
void tts_clear()
{
  TTS_POS = 0;
}
const char* tts_fill_final_buffer()
{
  int bi = 0;
  for (int i = 0; i < TTS_POS; i++)
  {
    if (TTS_MSG_BUILDER[i] == '\x01')
    {
      TTS_MSG_FINAL[bi++] = ' ';
    }
    else if (TTS_MSG_BUILDER[i] == '\x02')
    {  }
    else
    {
      TTS_MSG_FINAL[bi++] = TTS_MSG_BUILDER[i];
    }
  }
  TTS_MSG_FINAL[bi] = 0;
  return TTS_MSG_FINAL;
}
void destroy_tts() {}
#endif

void say_board()
{
  const int len = str_len(TTS_MSG_FINAL);
  if (len == 0)
    return;
  FILE* p = popen(TTS_CLI, "w");
  fwrite(TTS_MSG_FINAL, sizeof(char)*(len+1), 1, p);
  fclose(p);
}



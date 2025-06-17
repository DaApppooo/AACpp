#include "globals.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <raylib.h>
#include "tts.hpp"

constexpr int TTS_MSG_LEN = 2048;
char tts_msg_builder[TTS_MSG_LEN];
TTSMode tts_mode;
char tts_param[TTS_PARAM_MAX+1];
int tts_pos = 0;
Sound rlsound;
enum { SHIFT_OFF, SHIFT_ON, SHIFT_LOCK } shift = SHIFT_OFF; // shift key

char tts_msg_final[TTS_MSG_LEN];

#if defined(PLATFORM) && PLATFORM == PLATFORM_ANDROID
#error THIS PART IS TODO
#endif

void init_tts()
{
  memset(tts_msg_builder, 0, sizeof(char)*2048);
  // memset(tts_cli, 0, sizeof(char)*1024);
}
void tts_action(const char* action_name)
{
  if (str_eq(action_name, "shift"))
  {
    if (shift == SHIFT_OFF)
      shift = SHIFT_ON;
    else if (shift == SHIFT_ON)
      shift = SHIFT_LOCK;
    else
      shift = SHIFT_OFF;
  }
  else if (str_eq(action_name, "space"))
  {
    tts_push(" ");
  }
  else
  {
    TraceLog(LOG_WARNING, "Unknown special action '%s'.", action_name);
  }
}
void tts_push(const char* w)
{
  const int len = str_len(w);
  assert(len > 0);
  memcpy(
    tts_msg_builder + tts_pos,
    w,
    sizeof(char)*std::min(TTS_MSG_LEN - tts_pos - 1, len)
  );
  if (shift == SHIFT_ON || shift == SHIFT_LOCK)
  {
    for (int i = tts_pos; i < tts_pos + len; i++)
    {
      tts_msg_builder[i] = toupper(tts_msg_builder[i]);
    }
    if (shift == SHIFT_ON)
      shift = SHIFT_OFF;
  }
  tts_pos += len;
  tts_msg_builder[tts_pos] = '\x01';
  tts_pos++;
}
void tts_backspace()
{
  if (tts_pos > 1)
    tts_pos -= 2;
  while (
     tts_pos > 0
  && tts_msg_builder[tts_pos] != '\x01'
  ) {
      tts_pos--;
  }
}

void tts_play()
{
  switch (tts_mode)
  {
    case TTS_PIPER:
      {
        FILE* text = fopen("assets/text.txt", "w");
        if (!text)
        {
          TraceLog(LOG_ERROR, "[TTS] Failed to write text to file.");
          break;
        }
        fwrite(tts_msg_final, TTS_MSG_LEN, 1, text);
        fclose(text);
        const char* cmd;
        WIN(
          cmd = TextFormat("assets\\piper\\piper.exe -f assets\\voice.wav "
                           "-m %s -c %s.json < assets\\text.txt",
                         tts_param, tts_param);
        )
        LINUX(
          cmd = TextFormat("assets/piper/piper -f assets/voice.wav "
                           "-m %s -c %s.json < assets/text.txt",
                         tts_param, tts_param);
        )
        system(cmd);
        if (IsSoundPlaying(rlsound) || IsSoundValid(rlsound))
        {
          StopSound(rlsound);
          UnloadSound(rlsound);
        }
        rlsound = LoadSound("assets/voice.wav");
        PlaySound(rlsound);
        break;
      }
  }
}


void tts_clear()
{
  tts_pos = 0;
}

// fixed 4 byte "utf8" to real utf8
const char* tts_fill_final_buffer()
{
  constexpr int UTF8_CHECK_BIT = 1 << 7;
  int bi = 0;
  for (int i = 0; i < tts_pos; i++)
  {
    if (tts_msg_builder[i] == '\x01')
    { }
    else
    {
      tts_msg_final[bi++] = tts_msg_builder[i];
    }
  }
  tts_msg_final[bi] = '\n';
  tts_msg_final[bi+1] = 0;
  // because this function is equivalent to a tts update, we manage rlsound
  // here
  if (IsSoundValid(rlsound) && !IsSoundPlaying(rlsound))
  {
    UnloadSound(rlsound);
    rlsound.frameCount = 0;
    rlsound.stream.buffer = nullptr;
  }
  return (const char*)tts_msg_final;
}
void destroy_tts() {}




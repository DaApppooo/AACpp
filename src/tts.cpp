#include "../include/utils.hpp"
#include "../include/piper.hpp"
#include "tts.hpp"

constexpr int TTS_MSG_LEN = 2048;
FixedString::Char tts_msg_builder[TTS_MSG_LEN];
char tts_param[TTS_PARAM_MAX+1];
int tts_pos = 0;

char tts_msg_final[TTS_MSG_LEN];

#if defined(PLATFORM) && PLATFORM == PLATFORM_ANDROID
#error THIS PART IS TODO
#endif

#ifdef __linux__
// constexpr int TTS_CLI_LEN = 1024;
// char tts_cli[TTS_CLI_LEN];
void init_tts()
{
  memset(tts_msg_builder, 0, sizeof(FixedString::Char)*2048);
  // memset(tts_cli, 0, sizeof(char)*1024);
  
}
void tts_push(FixedString& w)
{
  const int len = w.len()-1;
  assert(len > 0);
  assert(w._data[0] == '+');
  memcpy(
    tts_msg_builder + tts_pos,
    w._data + 1,
    sizeof(FixedString::Char)*std::min(TTS_MSG_LEN - tts_pos - 1, len)
  );
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
        piper::PiperConfig config;
        piper::Voice voice;
        std::string text;
        std::ofstream audio_file;
        piper::SynthesisResult res;
        audio_file.open("assets/tmp.wav");
        piper::loadVoice(config, , std::string modelConfigPath, Voice &voice, std::optional<SpeakerId> &speakerId, bool useCuda)
        piper::textToWavFile(config, voice, TTS_MSG_FINAL, audio_file, res);
        break;
      }
  }
}
#endif


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
  tts_msg_final[bi] = 0;
  return (const char*)tts_msg_final;
}
void destroy_tts() {}




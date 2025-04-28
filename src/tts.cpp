#include "utils.hpp"
#include "piper.hpp"
#include <cstdlib>
#include <optional>
#include <raylib.h>
#include "tts.hpp"

constexpr int TTS_MSG_LEN = 2048;
FixedString::Char tts_msg_builder[TTS_MSG_LEN];
TTSMode tts_mode;
char tts_param[TTS_PARAM_MAX+1];
int tts_pos = 0;
Sound rlsound;

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
  setenv("ESPEAK_DATA_PATH", "assets/espeak-ng-data", 1);
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
        std::optional<piper::SpeakerId> spkid;
        piper::SynthesisResult res;
        piper::loadVoice(
          config, tts_param, std::string(tts_param)+".json",
          voice, spkid, false
        );
        if (voice.phonemizeConfig.phonemeType == piper::eSpeakPhonemes)
        {
          config.eSpeakDataPath = "assets/espeak-ng-data";
          config.useESpeak = true;
        }
        else
          config.useESpeak = false;
        if (voice.phonemizeConfig.eSpeak.voice == "ar")
        {
          TraceLog(LOG_ERROR, "Arabic (with tashkeel) is not yet supported.");
          return;
        }
        config.useTashkeel = false;
        printf("config.eSpeakDataPath='%s'\n", config.eSpeakDataPath.c_str());
        piper::initialize(config);
        audio_file.open("assets/tmp.wav", std::ios::binary);
        piper::textToWavFile(config, voice, tts_msg_final, audio_file, res);
        audio_file.close();
        TraceLog(LOG_INFO, "(lib)PIPER: Generated wave file successfuly.");
        rlsound = LoadSound("assets/tmp.wav"); // emits info msg
        if (rlsound.frameCount == 0)
          return;
        PlaySound(rlsound);
        piper::terminate(config);
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




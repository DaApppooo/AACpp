#include "utils.hpp"

const int TTS_PARAM_MAX = 1023;
enum TTSMode { TTS_PIPER };
extern TTSMode tts_mode;
extern char tts_param[TTS_PARAM_MAX+1];

void init_tts();
void tts_action(const char* action_name);
void tts_push(const char* w);
void tts_backspace(); // remove last pushed piece or word
void tts_clear();
const char* tts_fill_final_buffer();
void tts_play();
void destroy_tts();

inline void tts_change(const char* w)
{
  tts_backspace();
  tts_push(w);
}




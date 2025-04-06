#include "../include/utils.hpp"

const int TTS_PARAM_MAX = 1023;
enum TTSMode { TTS_PIPER };
extern TTSMode tts_mode;
extern char tts_param[TTS_PARAM_MAX+1];

void init_tts();
void tts_change(FixedString& w); // FUTURE: takes an index as parameter
void tts_push(FixedString& w);
void tts_backspace(); // remove last pushed piece or word
void tts_clear();
const char* tts_fill_final_buffer();
void tts_play();
void destroy_tts();

inline void tts_change(FixedString& w)
{
  tts_backspace();
  tts_push(w);
}




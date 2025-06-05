#ifndef H_WEBCOLOR
#define H_WEBCOLOR
#include "utils.hpp"
#include <raylib.h> // Just for Color

// enable flag DEBUG to have errors emited
Color color_parse_or(const char* s, Color fallback);

#endif

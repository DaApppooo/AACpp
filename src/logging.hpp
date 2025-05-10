
#include "raylib.h"
#include <cstdarg>
void init_logging();
void _log(int level, const char* format, va_list args);
void logging_update();
void destroy_logging();



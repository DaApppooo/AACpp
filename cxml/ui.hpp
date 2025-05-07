#define NO_CLAY
#include "clay.h"
#include "theme.hpp"
#include "settings.hpp"
#include "resman.hpp"

extern "C" {
void _layout_options(Clay_RenderCommandArray& rc);
void _layout_home(Clay_RenderCommandArray& rc);
}

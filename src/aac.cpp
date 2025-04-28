#include "resman.hpp"
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <raylib.h>
#include <raymath.h>
#define NO_CLAY
#include "clay.h"
#include "rlclay.h"
#include "theme.hpp"
#include "utils.hpp"
#include "board.hpp"
#include "tts.hpp"
#include "settings.hpp"
#include "globals.hpp"

inline void board_update(
  Clay_RenderCommandArray& render_cmds,
  opt_board_index_t& opt_new_board,
  board_index_t& current
);

const char* failure_reason = nullptr;
void loading_failure_screen();

void clay_error_handler(Clay_ErrorData error_data)
{
  TraceLog(LOG_ERROR, "[CLAY] %s (type: %i)", error_data.errorText.chars, (int)error_data.errorType);
}

int main()
{
  Clay_RenderCommandArray render_cmds;
  Clay_Context* clay_ctx;
  uint64_t clay_req_mem;
  void* allocated_mem;
  board_index_t current = 0;
  opt_board_index_t opt_new_board;
  float dt;

  {
    InitAudioDevice();
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    SetTargetFPS(15);
    InitWindow(1600, 900, "aacpp");
    XMAX = GetScreenWidth();
    YMAX = GetScreenHeight();
  }

  init_tts();
  if ((failure_reason = settings_load()))
  {
    loading_failure_screen();
    return EXIT_FAILURE;
  }
  assert(theme::fonts != nullptr);
  
  {
    clay_req_mem = Clay_MinMemorySize();
    allocated_mem = malloc(clay_req_mem);
    assert(allocated_mem != nullptr);
    clay_ctx = Clay_Initialize(
      Clay_CreateArenaWithCapacityAndMemory(clay_req_mem, allocated_mem),
      {XMAX, YMAX},
      {clay_error_handler}
    );
    Clay_SetCurrentContext(clay_ctx);
  }

  { // board and ressource loading
    load_layouts();
    if (init_res(current_board_file))
    {
      return EXIT_FAILURE;
    }
  }

  Clay_Raylib_Initialize(); // Should be the last thing to be initialized
  bool clay_debug = false;

  while (!WindowShouldClose())
  {
    dt = GetFrameTime();
    Vector2 mwm = GetMouseWheelMoveV();
    Clay_SetLayoutDimensions({XMAX, YMAX});
    Clay_SetPointerState({ctrl::mpos.x, ctrl::mpos.y}, ctrl::touch_press);
    Clay_UpdateScrollContainers(true, {mwm.x, mwm.y}, dt);

    if (settings_open)
    {
      settings_update(render_cmds);
    }
    else
    {
      board_update(render_cmds, opt_new_board, current);
    }
    Clay_SetDebugModeEnabled(clay_debug);

    if (IsKeyPressed(KEY_R))
    {
      load_layouts();
      TraceLog(LOG_INFO, "Reloaded layouts !");
    }
    if (IsKeyPressed(KEY_D))
    {
      clay_debug = !clay_debug;
    }

    // NOTE: Raylib provides compatibility with touch screen
    // without use of a different function
    ctrl::touch_press = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    ctrl::mpos = GetMousePosition();
    XMAX = GetScreenWidth();
    YMAX = GetScreenHeight();
    if (is_board_index(opt_new_board))
    {
      current = opt_new_board;
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
      if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("btn_parent"))))
      {
        if (is_board_index(board_with_index(current).parent))
        {
          current = board_with_index(current).parent;
          current_actions.init();
        }
      }
      if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("btn_options"))))
      {
        settings_show();
      }
      if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("btn_backspace"))))
      {
        current_actions.init();
        tts_backspace();
      }
      if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("btn_clear"))))
      {
        current_actions.init();
        tts_clear();
      }
      if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("btn_play"))))
      {
        tts_play();
      }
    }
  }

SAFELY_EXIT:

  free(allocated_mem);

  Clay_Raylib_Close();
  destroy_settings();
  destroy_res();
  destroy_tts();
  CloseWindow();
  
  return 0;
}

inline void board_update(
  Clay_RenderCommandArray& render_cmds,
  opt_board_index_t& opt_new_board,
  board_index_t& current
) {
  static bool debug = false;
  Clay_SetDebugModeEnabled(debug);
  if (IsKeyPressed(KEY_D))
    debug = ! debug;

  layout_home(render_cmds);

  // NOTE: update before drawing. Weird visual bugs can happen because of
  // possible animations.shearing
  // TODO: If no animations are necessary in productoin, remove left code
  // residues
  opt_new_board = board_with_index(current).update();
  BeginDrawing();
    ClearBackground(BLACK);
    Clay_Raylib_Render(render_cmds, theme::fonts);
    board_with_index(current).draw();
    // NOTE: BECAUSE CLAY HAS A TEXT BUG, I'LL THE THE TEXT MYSELF WITH RAYLIB
    DrawTextPro(
      theme::fonts[0],
      tts_fill_final_buffer(),
      {
        theme::gpad*2 + (theme::BAR_HEIGHT - theme::gpad*2)*2
        + theme::SAFETY_MARGIN + 10*3,
        theme::BAR_HEIGHT/2.f + theme::gpad
      },
      { 0.f, theme::font_size/1.5f },
      0.f,
      theme::font_size,
      theme::TEXT_SPACING,
      CLAY_COLOR_TO_RAYLIB_COLOR(theme::text_color)
    );
  EndDrawing();
}

void loading_failure_screen()
{
  if (failure_reason == nullptr)
    failure_reason = "Unknown failure reason.";
  while (!WindowShouldClose())
  {
    BeginDrawing();
      ClearBackground(CLAY_COLOR_TO_RAYLIB_COLOR(theme::background_color));
      const int w = MeasureText(failure_reason, 50);
      DrawText(failure_reason, int(XMAX)/2 - w/2, YMAX/2 - 25, 50,
        CLAY_COLOR_TO_RAYLIB_COLOR(theme::text_color));
    EndDrawing();
  }
  CloseWindow();
}



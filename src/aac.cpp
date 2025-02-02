#include "nfd.h"
#include "resman.hpp"
#include <csignal>
#include <cstdio>
#include <cwchar>
#include <raylib.h>
#include <raymath.h>
#define NO_CLAY
#include "clay.h"
#include "rlclay.h"
#include "theme.hpp"
#include "utils.hpp"
#include "board.hpp"
#include "proc.hpp"

inline float throbber_func(float x)
{
  static constexpr float speed = 4.f;
  return (sinf(speed*x)+1.f)/speed + x;
}

int main()
{
  Clay_RenderCommandArray render_cmds;
  Stream board_src;
  uint64_t clay_req_mem;
  void* allocated_mem;
  board_index_t current;
  opt_board_index_t opt_new_board;
  const char* temp;
  Proc child;
  char* child_param_buffer[5];

  {
    // SetConfigFlags(FLAG_FULLSCREEN_MODE);
    SetTargetFPS(15);
    Clay_Raylib_Initialize(1600, 900, "aacpp", FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    XMAX = GetScreenWidth();
    YMAX = GetScreenHeight();
  }

  init_tts();
  theme_load();

  {
    clay_req_mem = Clay_MinMemorySize();
    allocated_mem = malloc(clay_req_mem);
    assert(allocated_mem != nullptr);
    Clay_Initialize(
      Clay_CreateArenaWithCapacityAndMemory(clay_req_mem, allocated_mem),
      {XMAX, YMAX}
    );
  }

  { // board selection and compilation if necessary
    current = 0;
    assert(alloc_board() == 0);
    const char* const unknown_board_path = open_file_dialogue();
    const char* compiled_board_path;
    if (IsFileExtension(unknown_board_path, ".cobz"))
    {
      compiled_board_path = unknown_board_path;
    }
    else if (
       FileExists(temp=TextFormat("%s.cobz", unknown_board_path))
    && FileEditTime(unknown_board_path) <= FileEditTime(temp)
    )
    {
      // file was already auto compiled and there's no new version
      compiled_board_path = temp;
    }
    else
    {
      child_param_buffer[0] = (char*)"python3";
      child_param_buffer[1] = (char*)"obz2cobz.py";
      child_param_buffer[2] = (char*)unknown_board_path;
      child_param_buffer[3] = (char*)TextFormat("%s.cobz", unknown_board_path);
      child_param_buffer[4] = nullptr;
      child.launch(
        "python3",
        child_param_buffer
      );
      // a whole load ass screen
      float t = 0;
      float dt;
      while (!WindowShouldClose() && !child.ended())
      {
        XMAX = GetScreenWidth();
        YMAX = GetScreenHeight();
        BeginDrawing();
          ClearBackground(
            CLAY_COLOR_TO_RAYLIB_COLOR(theme::background_color)
          );
          const int w = MeasureText(
            "Compiling board... (done once per board set)",
            50
          );
          DrawText(
            "Compiling board... (done once per board set)",
            int(XMAX)/2 - w/2,
            YMAX/2 - 25,
            50,
            CLAY_COLOR_TO_RAYLIB_COLOR(theme::text_color)
          );
#define THROBBER(F) \
          DrawCircleV( \
            { \
              XMAX/2.f + cosf(throbber_func(t*(F))*2.f*PI)*20.f, \
              2.f*YMAX/3.f + sinf(throbber_func(t*(F))*2.f*PI)*20.f \
            }, \
            7.5f, \
            WHITE \
          )
        THROBBER(1);
        THROBBER(0.8);
        THROBBER(0.6);
        THROBBER(0.4);
        EndDrawing();
        dt = GetFrameTime();
        t += dt;
      }
      if (WindowShouldClose())
      {
        if (!child.ended())
        {
          child.kill();
        }
        goto SAFELY_EXIT;
      }
      // NOTE: we can't use 'temp' because we can't be 100% sure it was set
      //  because of possible compiler shenanigans.
      compiled_board_path = TextFormat("%s.cobz", unknown_board_path);
    }

    board_src = {fopen(compiled_board_path, "rb")};
    if (board_src._f == nullptr)
    {
      TraceLog(LOG_FATAL, "Could not open compiled open board file '%s'.", compiled_board_path);
      return EXIT_FAILURE;
    }
  }

  { // board and ressource loading
    if (init_res(board_src))
    {
      return EXIT_FAILURE;
    }
  }

  board_with_index(current).hold();
  while (!WindowShouldClose())
  {
    Clay_SetLayoutDimensions({XMAX, YMAX});
    Clay_SetPointerState({ctrl::mpos.x, ctrl::mpos.y}, ctrl::touch_press);
    Clay_BeginLayout();

      CLAY(
        CLAY_ID("body"),
        CLAY_RECTANGLE({ .color = theme::background_color }),
        CLAY_LAYOUT({
                    .sizing = theme::GROW,
                    .layoutDirection = CLAY_TOP_TO_BOTTOM
                  })
      ) {
        CLAY(
          CLAY_ID("header"),
          CLAY_RECTANGLE({theme::TRANSPARENT_RECT}),
          CLAY_LAYOUT({
                .sizing = theme::BAR_SIZE,
                .padding = theme::gpad,
                .childGap = 10,
                .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER},
                .layoutDirection = CLAY_LEFT_TO_RIGHT
          })
        ) {
          CLAY( // app options icon (possible additional board editing tools)
            CLAY_ID("btn_options"),
            CLAY_RECTANGLE({.color=theme::command_background}),
            CLAY_LAYOUT({
              .sizing = theme::ICON_SIZE
            }),
            CLAY_IMAGE({btns+BTI_OPT, theme::IMG_SCALE})
          ) {};
          CLAY( // SAFETY MARGIN
            theme::TRANSPARENT_RECT,
            CLAY_LAYOUT({
              .sizing = theme::SAFETY_MARGIN
            })
          ) {};
          CLAY( // parent board icon
            CLAY_ID("btn_parent"),
            CLAY_RECTANGLE({.color=theme::command_background}),
            CLAY_LAYOUT({
              .sizing = theme::ICON_SIZE
            }),
            CLAY_IMAGE({btns+BTI_UP, theme::IMG_SCALE})
          ) {};
          CLAY( // input text
            CLAY_RECTANGLE({.color=theme::text_field_bg_color}),
            CLAY_LAYOUT({
              .sizing = {.width = CLAY_SIZING_GROW(), .height = CLAY_SIZING_FIXED(100)},
              .padding = theme::gpad,
              .childGap = 10,
              .childAlignment = {CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER},
              .layoutDirection = CLAY_LEFT_TO_RIGHT,
            })
          ) {
            CLAY_TEXT(CLAY_STRING(tts_fill_final_buffer()), CLAY_TEXT_CONFIG(font));
          };
          CLAY( // backspace input icon
            CLAY_ID("btn_backspace"),
            CLAY_RECTANGLE({.color=theme::command_background}),
            CLAY_LAYOUT({
              .sizing = theme::ICON_SIZE
            }),
            CLAY_IMAGE({btns+BTI_BACKSPACE, theme::IMG_SCALE})
          ) {};
          CLAY( // clear input icon
            CLAY_ID("btn_clear"),
            CLAY_RECTANGLE({.color=theme::command_background}),
            CLAY_LAYOUT({
              .sizing = theme::ICON_SIZE
            }),
            CLAY_IMAGE({btns+BTI_CLEAR, theme::IMG_SCALE})
          ) {};
          CLAY( // SAFETY MARGIN
            theme::TRANSPARENT_RECT,
            CLAY_LAYOUT({
              .sizing = theme::SAFETY_MARGIN
            })
          ) {};
          CLAY( // play input icon
            CLAY_ID("btn_play"),
            CLAY_RECTANGLE({.color=theme::command_background}),
            CLAY_LAYOUT({
              .sizing = theme::ICON_SIZE
            }),
            CLAY_IMAGE({btns+BTI_PLAY, theme::IMG_SCALE})
          ) {
          };
        };
        CLAY(
          CLAY_ID("actions"),
          CLAY_RECTANGLE({theme::TRANSPARENT_RECT}),
          CLAY_LAYOUT({
                .sizing = theme::BAR_SIZE,
                .padding = theme::gpad,
                .childGap = 10,
                .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER},
                .layoutDirection = CLAY_LEFT_TO_RIGHT
          })
        ) {
          // Filled without clay
          // FUTURE: Add button IF overflow to scroll between options
        }
        CLAY(
          CLAY_ID("content"),
          CLAY_LAYOUT({
                  .sizing = theme::GROW,
                  .padding = {theme::gpad, theme::gpad},
                  .childGap = 10,
                  .childAlignment = {CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_TOP},
                  .layoutDirection = CLAY_LEFT_TO_RIGHT
          }),
          CLAY_SCROLL({
            .vertical = true
          })
        )
        {
          // FUTURE: use clay grid layout system
        };
      }

    render_cmds = Clay_EndLayout();

    // NOTE: update before drawing. Weird visual bugs can happen because of
    // possible animations.
    // TODO: If no animations are necessary in productoin, remove left code
    // residues
    opt_new_board = board_with_index(current).update();
    BeginDrawing();
      ClearBackground(BLACK);
      Clay_Raylib_Render(render_cmds);
      board_with_index(current).draw();
    EndDrawing();

    // NOTE: Raylib provides compatibility with touch screen
    // without use of a different function
    ctrl::touch_press = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    ctrl::mpos = GetMousePosition();
    XMAX = GetScreenWidth();
    YMAX = GetScreenHeight();
    if (is_board_index(opt_new_board))
    {
      board_with_index(current).drop();
      current = opt_new_board;
      board_with_index(current).hold();
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
      if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("btn_parent"))))
      {
        if (is_board_index(board_with_index(current).cells[0].parent))
        {
          board_with_index(current).drop();
          current = board_with_index(current).cells[0].parent;
          current_actions.init();
          board_with_index(current).hold();
        }
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
  board_with_index(current).drop();

SAFELY_EXIT:

  free(allocated_mem);
  
  destroy_res();
  destroy_tts();
  CloseWindow();
  
  return 0;
}




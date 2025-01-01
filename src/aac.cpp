#include "resman.hpp"
#include <cstdio>
#include <cwchar>
#include <raylib.h>
#include <raymath.h>
#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "rlclay.h"
#include "theme.hpp"
#include "utils.hpp"
#include "board.hpp"

int main()
{
  Clay_RenderCommandArray render_cmds;
  Stream board_src;
  uint64_t clay_req_mem;
  void* allocated_mem;
  board_index_t current;

  {
    // SetConfigFlags(FLAG_FULLSCREEN_MODE);
    SetTargetFPS(60);
    Clay_Raylib_Initialize(1600, 900, "aacpp", FLAG_WINDOW_RESIZABLE);
    XMAX = GetScreenWidth();
    YMAX = GetScreenHeight();
  }

  init_tts();

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
    else
    {
      system(TextFormat("py %s %s.cobz", unknown_board_path, unknown_board_path));
      compiled_board_path = TextFormat("%s.cobz", unknown_board_path);
    }
    board_src = {fopen(compiled_board_path, "")};
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
    fclose(board_src._f);
  }

  while (!WindowShouldClose())
  {
    Clay_SetLayoutDimensions({XMAX, YMAX});
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
                .childGap = 10,
                .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER},
                .layoutDirection = CLAY_LEFT_TO_RIGHT
          })
        ) {
          // TODO: ADD BUTTONS
        };
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
        CLAY(
          CLAY_ID("footer"),
          CLAY_LAYOUT({
                        .sizing = theme::BAR_SIZE,
                      }),
          CLAY_RECTANGLE({theme::TRANSPARENT_RECT})
        ) {}
      }

    render_cmds = Clay_EndLayout();
    
    BeginDrawing();
      ClearBackground(BLACK);
      Clay_Raylib_Render(render_cmds);

      board_with_index(current).draw();
    EndDrawing();

    ctrl::mouse_down = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    ctrl::mpos = GetMousePosition();
    XMAX = GetScreenWidth();
    YMAX = GetScreenHeight();
  }
  for (int i = 0; i < texs.len(); i++) {
    UnloadTexture(texs[i]);
  }

  free(allocated_mem); // this might crash
  
  destroy_res();
  destroy_tts();
  UnloadFont(Raylib_fonts[0].font);
  CloseWindow();
  
  return 0;
}




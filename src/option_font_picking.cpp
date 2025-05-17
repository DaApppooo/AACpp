#include "settings.hpp"
#include "theme.hpp"
#include "option_font_picking.hpp"

namespace font_picking
{
  constexpr float POPUP_UPSCALE = 1.2f;
  list<char*> available_font_paths;
  list<Font> available_fonts;
  float scroll;
  float max_w;
  char path[1024];
  void set_path(const char* p)
  { strncpy(path, p, 1024); }
  void init(Option&)
  {
    scroll = 0.f;
    char* search_dirs;
  #ifdef __linux__
    char* xdg_data_dirs = getenv("XDG_DATA_DIRS");
    search_dirs = xdg_data_dirs
                      ? xdg_data_dirs
                      : (char*)TextFormat(
                       "/usr/share:/usr/local/share:%s/.local/share",
                       getenv("HOME"));
  #else
  # error init_font_picking for windows, android, ios: TODO
  #endif
    available_font_paths.init();
    { // First load packaged font
      FilePathList fpl = LoadDirectoryFilesEx(
        "assets",
        ".ttf", true
      );
      available_font_paths.extend({
        .start = fpl.paths,
        .len = fpl.count,
        .full = fpl.count
      });
      for (int i = 0; i < fpl.count; i++)
        fpl.paths[i] = 0; // disown strings
      UnloadDirectoryFiles(fpl);
    }

    // Then load system fonts
    char* start = search_dirs;
    char* end = search_dirs;
    while (start != nullptr)
    {
      while (*end != ':' && *end != 0)
        end++;
      char* const next_start = (*end) ? end+1 : nullptr;
      *end = 0;
      FilePathList fpl = LoadDirectoryFilesEx(
        TextFormat("%s/fonts",start),
        ".ttf", true
      );
      available_font_paths.extend({
        .start = fpl.paths,
        .len = fpl.count,
        .full = fpl.count
      });
      for (int i = 0; i < fpl.count; i++)
        fpl.paths[i] = 0; // disown strings
      UnloadDirectoryFiles(fpl);
      start = next_start;
      if (next_start != nullptr)
        *end = ':';
      end++;
    }
    available_fonts.prealloc(available_font_paths.len());
    max_w = 0.f;
    int len = available_font_paths.len();
    for (int i = 0; i < len; i++)
    {
      const char* name = filename(available_font_paths[i]);
      if (
        !isalpha(name[0])
      || name[1] == 0
      || !isalpha(name[1])
      ) {
        available_font_paths.rmv(i);
        i--;
        len--;
        continue;
      }
      Font font = LoadFontEx(
        available_font_paths[i],
        int(theme::font_size*POPUP_UPSCALE), NULL, 0
      );
      if (font.texture.id == 0
          // || font.baseSize > theme::font_size*UI_UPSCALE
          || MeasureTextEx(
              font, name,
              theme::font_size, theme::TEXT_SPACING
            ).x < str_len(name)*theme::font_size/3.f
      ) {
        UnloadFont(font);
        available_font_paths.rmv(i);
        i--;
        len--;
        continue;
      }
      available_fonts.push(font);
      const float m = MeasureTextEx(
        available_fonts[i],
        filename(available_font_paths[i]),
        theme::font_size*POPUP_UPSCALE,
        theme::TEXT_SPACING
      ).x;
      if (m > max_w)
        max_w = m;
    }
  }
  void destroy(Option&)
  {
    for (int i = 0; i < available_font_paths.len(); i++)
    {
      free(available_font_paths[i]);
      UnloadFont(available_fonts[i]);
    }
    available_font_paths.destroy();
    available_fonts.destroy();
  }
  void mainbox(Option& self, Rectangle box)
  {
    DrawTextEx(
      theme::fonts[0],
      "Current font family",
      {box.x+theme::gpad, box.y+box.height/2.f-theme::font_size},
      theme::font_size, theme::TEXT_SPACING,
      ~theme::text_color
    );
  }
  Rectangle popup(Option& self)
  {
    constexpr int SHOW_COUNT = 8;
    const float fs = theme::font_size*POPUP_UPSCALE;
    const Rectangle r = find_valid_rect(max_w, fs*SHOW_COUNT);
    DrawRectangleRounded(r, 0.1f, 16, ~theme::background_color);
    scroll += ctrl::delta_scroll_in(r);
    if (scroll > (available_font_paths.len()-SHOW_COUNT)*fs)
      scroll = (available_font_paths.len()-SHOW_COUNT)*fs;
    if (scroll < 0.f)
      scroll = 0.f;
    const float h = fs;

    BeginScissorMode(r.x, r.y, r.width, r.height);
    for (int i = 0; i < SHOW_COUNT+1; i++)
    {
      const int idx = i + scroll/h;
      if (idx < 0)
        continue;
      if (idx >= available_font_paths.len())
        break;
      const fvec2 m = MeasureTextEx(
        available_fonts[idx],
        filename(available_font_paths[idx]),
        fs,
        theme::TEXT_SPACING
      );
      DrawTextEx(
        available_fonts[idx],
        filename(available_font_paths[idx]),
        {r.x+theme::gpad, r.y+idx*h-scroll},
        fs,
        theme::TEXT_SPACING,
        ~theme::text_color
      );
      const Rectangle ir = {r.x, r.y+idx*h-scroll, r.width, h};
      if (ctrl::hover(ir))
      {
        DrawRectangleRec(ir, Fade(~theme::text_color, 0.1f));
        if (ctrl::clicked())
        {
          set_path(available_font_paths[idx]);
          UnloadFont(theme::fonts[0]);
          theme::fonts[0] = LoadFontEx(available_font_paths[idx],
                                     theme::font_size, NULL, 0);
          destroy(self);
          current_options = -1;
        }
      }
    }
    EndScissorMode();

    DrawRectangleRoundedLinesEx(
      r, 0.1f, 16,
      theme::border_weight, ~theme::text_color
    );
    return r;
  }
}


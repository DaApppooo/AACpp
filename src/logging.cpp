#include "logging.hpp"
#include "raylib.h"
#include "theme.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include "globals.hpp"

void init_logging()
{
  SetTraceLogCallback(&_log);
}

struct Log
{
  static constexpr float LIFETIME = 3.f;
  char info[1024];
  float lifetime;
  TraceLogLevel level;
};
struct LogQueue
{
  static constexpr int MAX = 12;
  Log data[MAX];
  int head;
  int len;
  void init()
  {
    head = len = 0;
  }
  void pop()
  {
    assert(len > 0);
    len--;
  }
  Log& push()
  {
    const int i = head;
    head++;
    if (head >= MAX)
      head = 0;
    len++;
    if (len > MAX)
      len = MAX;
    return data[i];
  }
  Log& last()
  {
    const int i = head - 1;
    if (i < 0)
      return data[MAX-1];
    else
      return data[i];    
  }
  Log& first()
  {
    const int i = head - len;
    if (i < 0)
      return data[MAX+i];
    else
      return data[i];
  }
  struct Iter
  {
    LogQueue& parent;
    int _i;
    Iter next() const { return {parent, _i+1}; }
    int index() const { return _i; }
    Log& value()
    {
      const int j = parent.head - _i - 1;
      if (j < 0)
        return parent.data[MAX+j];
      else
        return parent.data[j];
    }
  };
  Iter end()
  {
    return {*this, len};
  }
  Iter iter()
  {
    return {*this, 0};
  }
} logs;

bool operator == (LogQueue::Iter a, LogQueue::Iter b)
{
  return &a.parent == &b.parent && a.index() == b.index();
}
bool operator != (LogQueue::Iter a, LogQueue::Iter b)
{ return !(a == b); }
LogQueue::Iter operator ++ (LogQueue::Iter& a)
{
  return {a.parent, ++a._i};
}

void _log(int level, const char *format, va_list args)
{
  Log& l = logs.push();
  memset(l.info, 0, 1024);
  vsnprintf(l.info, 1024, format, args);
  l.level = (TraceLogLevel)level;
  l.lifetime = Log::LIFETIME;
  switch (level)
  {
    case LOG_WARNING: printf("\e[95;0mWARN"); break;
    case LOG_INFO: printf("\e[0mINFO"); break;
    case LOG_ERROR: printf("\e[31;0mERROR"); break;
    case LOG_FATAL: printf("\e[91;0mFATAL"); break;
    case LOG_DEBUG: printf("\e[93;0mDEBUG"); break;
    case LOG_TRACE: printf("\e[32;0mTRACE"); break;
    default:
      printf("?[%i]", level);
      break;
  }
  printf("\e[0m: %s\n", l.info);
}

void logging_update()
{
  const Color ibg = ~theme::background_color;
  const Color bg = rgb(255-ibg.r, 255-ibg.g, 255-ibg.b);
  const float H = theme::font_size*2.f+theme::gpad*2.f;
  for (auto it = logs.iter(); it != logs.end(); ++it)
  {
    const fvec2 m = MeasureTextEx(
      theme::fonts[0],
      it.value().info,
      theme::font_size,
      theme::TEXT_SPACING
    );
    const float w = std::max(m.x, 100.f);
    const Rectangle r = {
      float(theme::gpad),
      it.index() * (H+theme::gpad),
      w+theme::gpad*2.f,
      H
    };
    const char* level = "???";
    Color color = GREEN;
    switch (it.value().level)
    {
      case LOG_DEBUG: level = "DEBUG"; color = rgb(255,255,0); break;
      case LOG_ERROR: level = "ERROR"; color = rgb(255,0,0); break;
      case LOG_FATAL: level = "FATAL"; color = rgb(255,0,0); break;
      case LOG_WARNING: level = "WARN"; color = rgb(255, 127, 0); break;
      case LOG_INFO: level = "INFO"; color = ~theme::background_color; break;
      case LOG_TRACE: level = "TRACE"; color = rgb(127,127,127); break;
      default:
        break;
    }
    const float f = std::min(2.f*it.value().lifetime/Log::LIFETIME, 1.f);
    DrawRectangleRec(r, fade(bg, f));
    DrawTextEx(
      theme::fonts[0],
      level,
      {r.x+theme::gpad, r.y+theme::gpad},
      theme::font_size,
      theme::TEXT_SPACING,
      fade(color, f)
    );
    DrawTextEx(
      theme::fonts[0],
      it.value().info,
      {r.x+theme::gpad, r.y+theme::font_size+theme::gpad},
      theme::font_size,
      theme::TEXT_SPACING,
      fade((~theme::background_color), f)
    );
    it.value().lifetime -= dt;
  }
  while (logs.len > 0 && logs.first().lifetime <= 0.f)
    logs.pop();
}

void destroy_logging()
{
  
}


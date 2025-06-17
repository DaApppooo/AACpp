#include "webcolor.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <raylib.h>
#include <string>

#ifdef DEBUG
#define WARN(...) TraceLog(LOG_WARNING, __VA_ARGS__)
#else
#define WARN(...)
#endif

inline int hex1_to_int(const char* const p)
{
  const char c = *p;
  if ('0' <= c && c <= '9')
    return c - '0';
  if ('a' <= c && c <= 'f')
    return c - 'a' + 10;
  if ('A' <= c && c <= 'F')
    return c - 'A' + 10;
  return -1;
}
inline int hex2_to_int(const char* const p)
{
  const int h = hex1_to_int(p);
  const int l = hex1_to_int(p+1);
  if (h < 0 || l < 0)
    return -1;
  return (h << 4) | l;
}
inline int atou8(const char* p)
{
  const int len = str_len(p);
  if (len == 0)
    return -1;
  char* endp;
  int norm_out = -1; // between -1 and 255
  float v = strtof(p, &endp);
  if (endp == p)
    return -1;
  while (*p != 0)
  {
    if (*p == '.')
      norm_out = 255*v;
    p++;
  }
  if (norm_out < 0)
    norm_out = int(v);
  if (norm_out < 0 || norm_out > 255)
    return -1;
  return norm_out;
}

Color color_parse_or(const char* s, Color fallback)
{
  const char* const o = s;
  if (s == nullptr)
    return fallback;
  if (*s == 0)
    return fallback;
  while (isspace(*s))
    s++;
  int len = str_len(s);
  while (isspace(s[len-1]))
    len--;
  Color ret = fallback;
  if (len < 4)
  {
    WARN("[WEBCOLOR] Color expression must be at least 4 character long.");
    return ret;
  }
  if (s[0] == '#')
  {
    switch (len)
    {
      case 1+3: // #RGB (3 component, each doubled)
        {
          const int r = hex1_to_int(s+1);
          const int g = hex1_to_int(s+2);
          const int b = hex1_to_int(s+3);
          if (r < 0 || g < 0 || b < 0)
          {
            WARN("[WEBCOLOR] Color hex [3 digit] format got unknown digits (accepted match [0-9a-zA-Z]).");
            return ret;
          }
          ret.r = r | (r << 4);
          ret.g = g | (g << 4);
          ret.b = b | (b << 4);
          break;
        }
      case 1+6: // #RRGGBB
        {
          const int r = hex2_to_int(s+1);
          const int g = hex2_to_int(s+1+2);
          const int b = hex2_to_int(s+1+4);
          if (r < 0 || g < 0 || b < 0)
          {
            WARN("[WEBCOLOR] Color hex [6 digit] format got unknown digits (accepted match [0-9a-zA-Z]).");
            return ret;
          }
          ret.r = r;
          ret.g = g;
          ret.b = b;
          break;
        }
      case 1+8: // #RRGGBBAA
        {
          const int r = hex2_to_int(s+1);
          const int g = hex2_to_int(s+1+2);
          const int b = hex2_to_int(s+1+4);
          const int a = hex2_to_int(s+1+6);
          if (r < 0 || g < 0 || b < 0 || a < 0)
          {
            WARN("[WEBCOLOR] Color hex [8 digit] format got unknown digits (accepted match [0-9a-zA-Z]).");
            return ret;
          }
          ret.r = r;
          ret.g = g;
          ret.b = b;
          ret.a = a;
          break;
        }
      default:
        WARN("[WEBCOLOR] Unknown hex color format: '%s'. Must have 3, 6 or 8 components.", s);
        return ret;
    }
  }
  else if (
     s[0] == 'r'
  && s[1] == 'g'
  && s[2] == 'b'
  && s[3] == '('
  ) {
    int components[3];
    s += 4;
    for (int i = 0; i < 3; i++)
    {
      // Modifications will be reverted back
      char* comma = (char*)s;
      while (*comma != ',' && *comma != ')' && *comma != 0)
        comma++;
      if (*comma == 0 || (*comma == ')' && i < 2))
      {
        WARN("[WEBCOLOR] Invalid rgb format.");
        return ret;
      }
      *comma = 0;
      components[i] = atou8(s);
      if (i == 2)
        *comma = ')'; 
      else
        *comma = ','; 
      s = comma+1;
    }
    ret.r = components[0];
    ret.g = components[1];
    ret.b = components[2];
  }
  else if (
     len > 5
  && s[0] == 'r'
  && s[1] == 'g'
  && s[2] == 'b'
  && s[3] == 'a'
  && s[4] == '('
  ) {
    int components[4];
    s += 5;
    for (int i = 0; i < 4; i++)
    {
      // Modifications will be reverted back
      char* comma = (char*)s;
      while (*comma != ',' && *comma != ')' && *comma != 0)
        comma++;
      if (*comma == 0 || (*comma == ')' && i < 2))
      {
        WARN("[WEBCOLOR] Invalid rgba format.");
        return ret;
      }
      *comma = 0;
      components[i] = atou8(s);
      if (i == 2)
        *comma = ')'; 
      else
        *comma = ','; 
      s = comma+1;
    }
    ret.r = components[0];
    ret.g = components[1];
    ret.b = components[2];
    ret.a = components[3];
  }
  else
    TraceLog(LOG_WARNING, "[WEBCOLOR] Unknown color expression '%s'.", o);
  return ret;
}


#ifndef H_OPTIONS
/*
  Options GUI.
*/

extern bool options_open; // could be made float for animations
inline void options_show()
{
  options_open = true;
}
void init_options();
void options_update(); // includes drawing
void destroy_options();

#endif

/*
NOTE: This is (with rlclay.cpp) the code provided as a render engine example
for clay with raylib, but I modified it just so that I can compile it with
everything else. No code was changed, I just moved the actual code in
rlclay.cpp and kept the header a simple header.
*/
#define NO_CLAY
#include "clay.h"
#include "utils.hpp"
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <utility>
#ifdef CLAY_OVERFLOW_TRAP
#include <signal.h>
#endif

#ifndef CLAY__MIN
#define CLAY__MIN(A, B) std::min((A), decltype(A)(B))
#endif
#ifndef CLAY__MAX
#define CLAY__MAX(A, B) std::max((A), decltype(A)(B))
#endif

#define CLAY_RECTANGLE_TO_RAYLIB_RECTANGLE(rectangle) (Rectangle) { .x = rectangle.x, .y = rectangle.y, .width = rectangle.width, .height = rectangle.height }
#define CLAY_COLOR_TO_RAYLIB_COLOR(color) (Color) { .r = (unsigned char)roundf(color.r), .g = (unsigned char)roundf(color.g), .b = (unsigned char)roundf(color.b), .a = (unsigned char)roundf(color.a) }

extern "C" {

// extern Camera Raylib_camera;
// extern uint32_t measureCalls;

typedef enum
{
    CUSTOM_LAYOUT_ELEMENT_TYPE_3D_MODEL
} CustomLayoutElementType;

typedef struct
{
    Model model;
    float scale;
    Vector3 position;
    Matrix rotation;
} CustomLayoutElement_3DModel;

typedef struct
{
    CustomLayoutElementType type;
    union {
        CustomLayoutElement_3DModel model;
    };
} CustomLayoutElement;

Ray GetScreenToWorldPointWithZDistance(
    Vector2 position,
    Camera camera,
    int screenWidth,
    int screenHeight,
    float zDistance
);

void Clay_Raylib_Initialize();
void Clay_Raylib_Close();

void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands, Font* fonts);

}



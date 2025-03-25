#include "ui.hpp"
extern "C" {void layout_options(Clay_RenderCommandArray& rc) {
Clay_BeginLayout();CLAY({.layout={.sizing={.width=CLAY_SIZING_GROW(0),.height=CLAY_SIZING_GROW(0)},.layoutDirection=CLAY_TOP_TO_BOTTOM},.backgroundColor=theme::background_color}){
  CLAY({.layout={.sizing={.width=CLAY_SIZING_GROW(0),.height=CLAY_SIZING_FIXED(theme::BAR_HEIGHT)},.layoutDirection=CLAY_LEFT_TO_RIGHT},.backgroundColor=theme::command_background}){
    CLAY({.layout={.sizing={.width=CLAY_SIZING_FIXED(50),.height=CLAY_SIZING_GROW(0)}},.backgroundColor={0,0,0,0}}){}CLAY({.id=CLAY_ID("btn_back"),.layout={.sizing={.width=CLAY_SIZING_FIXED(theme::BAR_HEIGHT),.height=CLAY_SIZING_FIXED(theme::BAR_HEIGHT)}},.backgroundColor={255,255,255,255},.image={.imageData=btns+BTI_BACKSPACE,.sourceDimensions=theme::IMG_SCALE}}){}}CLAY({.layout={.sizing={.width=CLAY_SIZING_GROW(0),.height=CLAY_SIZING_GROW(0)},.layoutDirection=CLAY_LEFT_TO_RIGHT}}){
    CLAY({.layout={.sizing={.width=CLAY_SIZING_GROW(0),.height=CLAY_SIZING_GROW(0)}},.backgroundColor={0,0,0,0}}){}CLAY({.layout={.sizing={.width=CLAY_SIZING_FIXED(800),.height=CLAY_SIZING_GROW(0)},.layoutDirection=CLAY_TOP_TO_BOTTOM},.scroll={.vertical=true}}){


    
    }CLAY({.layout={.sizing={.width=CLAY_SIZING_GROW(0),.height=CLAY_SIZING_GROW(0)}},.backgroundColor={0,0,0,0}}){}}}rc = Clay_EndLayout();}}
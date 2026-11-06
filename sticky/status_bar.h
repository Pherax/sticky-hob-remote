#pragma once
#include <cmath>
#include <algorithm>
#include "ui_state.h"
#include "esphome/components/display/display.h"
namespace sticky {
// Quiet status: icons appear only when something needs attention.
//   Wi-Fi off/lost (on control pages), hob link stale (on the cooktop page), battery below 20 %.
template<class D> void draw_status(D &d,esphome::display::BaseFont *icons,esphome::display::BaseFont *small,bool wifi,float battery,uint32_t now) {
 using namespace esphome::display;
 int x=794;
 if(std::isfinite(battery) && battery<20) {
   int percent=std::max(0,int(battery+0.5f));
   d.printf(x,28,small,COLOR_OFF,TextAlign::CENTER_RIGHT,"%d%%",percent);
   x-=48;
   d.rectangle(x,19,28,18,COLOR_OFF);d.filled_rectangle(x+28,24,3,8,COLOR_OFF);
   int fill=(percent*22)/100;if(fill)d.filled_rectangle(x+3,22,fill,12,COLOR_OFF);
   x-=20;
 }
 if(!ui.controls) return;
 if(!wifi) { d.print(x-14,28,icons,COLOR_OFF,TextAlign::CENTER,"\U000F05AA"); x-=36; }
 else if(ui.page==2 && !ui.fresh(now)) d.print(x-14,28,icons,COLOR_OFF,TextAlign::CENTER,"\U000F0338");
}
}

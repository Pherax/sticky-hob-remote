#pragma once
#include <cmath>
#include "ui_state.h"
#include "power.h"
#include "connection_state.h"
#include "esphome/components/display/display.h"
namespace sticky {
// MDI 7.4.47: stove, fan, coffee-outline, timer-outline, lightbulb-outline, cog-outline.
static const char *menu_glyphs[]={"\U000F04DE","\U000F0210","\U000F06CA","\U000F051B","\U000F0336","\U000F08BB"};
template<class D> void outline(D &d,Rect r){d.rectangle(r.x,r.y,r.w,r.h,esphome::display::COLOR_OFF);}
template<class D> void icon(D &d,esphome::display::BaseFont *font,int type,int x,int y) {
 if(type==0) {
   // Four equal rings, inspired by mdi:google-circles-extended.
   for(int dx:{-23,23})for(int dy:{-23,23}) {
     d.filled_circle(x+dx,y+dy,19,esphome::display::COLOR_OFF);
     d.filled_circle(x+dx,y+dy,12,esphome::display::COLOR_ON);
   }
   return;
 }
 d.print(x,y,font,esphome::display::COLOR_OFF,esphome::display::TextAlign::CENTER,menu_glyphs[type]);
}
template<class D> void draw_page(D &d, esphome::display::BaseFont *small, esphome::display::BaseFont *medium,
 esphome::display::BaseFont *large, esphome::display::BaseFont *clock_font,esphome::display::BaseFont *mdi,esphome::display::BaseFont *mdi_small,uint32_t now,int power_mode,float battery) {
 auto &s=ui;
 // Borderless: icons and text carry the layout; touch rectangles are unchanged.
 d.print(back_button.mx(),back_button.my(),mdi_small,esphome::display::COLOR_OFF,esphome::display::TextAlign::CENTER,"\U000F004D");
 if(s.page==1) {
   for(int i=0;i<6;i++) {auto r=menu_button(i);icon(d,mdi,i,r.mx(),r.my());}
   return;
 }
 if(s.page==5) {
   icon(d,mdi,3,400,115);
   uint32_t mins=s.timer_running?(s.remaining(now)+59)/60:s.timer_minutes;
   d.printf(400,240,clock_font,esphome::display::COLOR_OFF,esphome::display::TextAlign::CENTER,"%02u",unsigned(s.timer_done?0:mins));
   d.print(400,310,small,esphome::display::COLOR_OFF,esphome::display::TextAlign::CENTER,s.timer_done?"Timer finished":s.timer_running?"Minutes remaining":"Minutes");
   for(int i=0;i<3;i++)outline(d,timer_button(i));
   d.print(150,380,large,esphome::display::COLOR_OFF,esphome::display::TextAlign::CENTER,"-");
   d.print(400,380,medium,esphome::display::COLOR_OFF,esphome::display::TextAlign::CENTER,s.timer_running?"Cancel":"Start");
   d.print(650,380,large,esphome::display::COLOR_OFF,esphome::display::TextAlign::CENTER,"+");
   d.print(400,450,small,esphome::display::COLOR_OFF,esphome::display::TextAlign::CENTER,"Reminder only - does not control heating");return;
 }
 if(s.page==7) {
   d.print(400,40,medium,esphome::display::COLOR_OFF,esphome::display::TextAlign::CENTER,"Settings");
   for(int i=0;i<4;i++)outline(d,setting_button(i));
   d.print(48,110,medium,esphome::display::COLOR_OFF,"Hob connection  >");
   d.print(40,150,small,esphome::display::COLOR_OFF,short_address(connection.address).c_str());
   d.print(40,220,medium,esphome::display::COLOR_OFF,power_label(power_mode));
   d.print(40,310,medium,esphome::display::COLOR_OFF,"Sync clock");
   d.print(40,400,medium,esphome::display::COLOR_OFF,s.maintenance(now)?"Update mode active":"Enable updates (10 min)");
   return;
 }
 if(s.page==8 || s.page==9) {
   auto &c=connection;
   d.print(400,40,medium,esphome::display::COLOR_OFF,esphome::display::TextAlign::CENTER,s.page==8?"Hob connection":"Enter hob IP : port");
   if(s.page==8) {
     d.print(40,85,small,esphome::display::COLOR_OFF,short_address(c.address).c_str());
     outline(d,connect_button(0));d.print(70,135,medium,esphome::display::COLOR_OFF,"Detect hobs");
     outline(d,connect_button(1));d.print(450,135,medium,esphome::display::COLOR_OFF,"Enter IP");
     for(size_t i=0;i<c.found.size();i++) {
       outline(d,result_button(i));
       d.print(50,201+i*55,small,esphome::display::COLOR_OFF,short_address(c.found[i]).c_str());
     }
   } else {
     d.print(400,100,medium,esphome::display::COLOR_OFF,esphome::display::TextAlign::CENTER,c.input.empty()?"192.168.1.100":c.input.c_str());
     for(int i=0;i<15;i++) {
       auto r=keypad_button(i);int x=r.x,y=r.y;
       outline(d,r);
       d.print(x+86,y+27,medium,esphome::display::COLOR_OFF,esphome::display::TextAlign::CENTER,keys[i]);
     }
   }
   d.print(40,425,small,esphome::display::COLOR_OFF,c.message.c_str());
   return;
 }
 if(appliance_count(s.page)) {
   if(!s.ha_notice.empty())d.print(400,455,small,esphome::display::COLOR_OFF,esphome::display::TextAlign::CENTER,s.ha_notice.c_str());
   const char *hood[]={"Light","Timer","Off","+","-"};
   const char *coffee[]={"1 cup","2 cups"};
   const char *lights[]={"Kitchen top","Kitchen bottom","Hood"};
   for(int i=0;i<appliance_count(s.page);i++) {
     auto r=appliance_button(s.page,i);
     icon(d,mdi,s.page==4?2:s.page==6?4:1,r.mx(),r.my()-30);
     d.print(r.mx(),r.my()+48,medium,esphome::display::COLOR_OFF,esphome::display::TextAlign::CENTER,s.page==3?hood[i]:s.page==4?coffee[i]:lights[i]);
   }
 }
}
}

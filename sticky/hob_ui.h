#pragma once
#include "menu.h"
namespace sticky {
using esphome::display::COLOR_ON;   // white (paper)
using esphome::display::COLOR_OFF;  // black (ink)
using esphome::display::TextAlign;
// Scanline edges: same rounded hit geometry, without scanning every interior pixel.
inline int rounded_left(Rect r,int radius,int y) {
 int lo=r.x,hi=r.mx();
 while(lo<hi){int mid=(lo+hi)/2;if(rounded_contains(r,radius,mid,y))hi=mid;else lo=mid+1;}
 return lo;
}
template<class D> void rounded_outline(D &d,Rect r,int radius,int width=1) {
 for(int y=r.y;y<r.y+r.h;y++) {
  int l=rounded_left(r,radius,y),rr=r.x+r.w-1-(l-r.x);
  if(y<r.y+width||y>=r.y+r.h-width){d.line(l,y,rr,y,COLOR_OFF);continue;}
  int edge=std::max(l,std::max(rounded_left(r,radius,y-1),rounded_left(r,radius,y+1))-1)+width-1;
  d.line(l,y,edge,y,COLOR_OFF);
  d.line(r.x+r.w-1-(edge-r.x),y,rr,y,COLOR_OFF);
 }
}
// Fill one logical zone in ink. The left pair shares one rounded outline.
template<class D> void fill_zone(D &d,int z) {
 auto r=zone_button(z);auto outer=z<2?left_zone:r;int radius=z<2?40:36;
 for(int y=r.y;y<r.y+r.h;y++) {
  int l=rounded_left(outer,radius,y),rr=outer.x+outer.w-1-(l-outer.x);
  d.line(l,y,rr,y,COLOR_OFF);
 }
}
// Black-and-white only: partial refreshes stay fast and flash-free.
//   selected zone      -> solid black, white digit
//   zone at 0          -> empty outline (selected: shows 0)
//   residual heat      -> "H"
//   requested level    -> outlined box in the level row until the hob reports it
template<class D> void draw_hob(D &d,esphome::display::BaseFont *zone_font,esphome::display::BaseFont *large,
 esphome::display::BaseFont *medium,esphome::display::BaseFont *small,esphome::display::BaseFont *icons,
 esphome::display::BaseFont *small_icons,uint32_t now) {
 auto &s=ui;const bool fresh=s.fresh(now);

 // Power: inverted while on; a ring while switching on is pending.
 auto p=power_button;
 if(fresh && s.on()) {
   d.filled_circle(p.mx(),p.my(),32,COLOR_OFF);
   d.print(p.mx(),p.my(),icons,COLOR_ON,TextAlign::CENTER,"\U000F0425");
 } else {
   if(s.pending_on) { d.circle(p.mx(),p.my(),32,COLOR_OFF); d.circle(p.mx(),p.my(),31,COLOR_OFF); }
   d.print(p.mx(),p.my(),icons,COLOR_OFF,TextAlign::CENTER,"\U000F0425");
 }

 fill_zone(d,s.selected);
 rounded_outline(d,left_zone,40,2);
 for(int z=2;z<5;z++) rounded_outline(d,zone_button(z),36,2);
 // Divider of the left pair: broken around a selected half so the black shape stays clean.
 d.line(left_zone.x+20,224,left_zone.x+left_zone.w-21,224,s.selected<2?COLOR_ON:COLOR_OFF);

 for(int z=0;z<5;z++) {
   auto r=zone_button(z);
   const bool sel=z==s.selected;
   const auto ink=sel?COLOR_ON:COLOR_OFF;
   const int level=fresh?s.levels[z]:-1;
   const bool hot=s.hot[z]=="Hot";            // kept through a lost link: residual heat matters
   const bool pan=fresh && s.pans[z]=="Present";
   if(!fresh) d.print(r.mx(),r.my(),large,ink,TextAlign::CENTER,"?");
   else if(level>0 || sel) d.print(r.mx(),r.my()-8,zone_font,ink,TextAlign::CENTER,level_label(level).c_str());
   else if(hot) d.print(r.mx(),r.my()-8,large,ink,TextAlign::CENTER,"H");
   if(pan) d.print(r.mx(),r.y+r.h-28,small_icons,ink,TextAlign::CENTER,"\U000F02E5");
   // A selected zone at 0 shows its digit, so residual heat moves to a small corner mark.
   if(hot && sel && level<=0) d.print(r.mx()+r.w/2-28,r.y+24,small,ink,TextAlign::CENTER,"H");
 }

 // Reserved feature icons (multi-zone, pause, bridge): no commands yet.
 for(int i=0;i<3;i++) {
   auto r=feature_button(i);int x=r.x,y=r.y;
   if(i==0) {for(int a=0;a<2;a++)for(int b=0;b<2;b++)d.rectangle(x+8+a*26,y+8+b*26,22,22,COLOR_OFF);}
   if(i==1) {d.rectangle(x+14,y+8,12,48,COLOR_OFF);d.rectangle(x+38,y+8,12,48,COLOR_OFF);}
   if(i==2) {d.rectangle(x+6,y+8,22,48,COLOR_OFF);d.rectangle(x+36,y+8,22,48,COLOR_OFF);d.line(x+28,y+32,x+36,y+32,COLOR_OFF);}
 }

 // Level row for the selected zone. Solid = reported by the hob, outline = requested.
 const int reported=fresh?s.levels[s.selected]:-1;
 const int requested=s.req_zone==s.selected?s.req_level:-1;
 for(int n=0;n<=10;n++) {
   auto r=level_button(n);
   const int bx=r.x+5,by=r.y+8,bw=r.w-10,bh=r.h-16;
   const bool solid=reported==n;
   if(solid) d.filled_rectangle(bx,by,bw,bh,COLOR_OFF);
   if(requested==n && !solid) for(int t=0;t<3;t++) d.rectangle(bx+t,by+t,bw-2*t,bh-2*t,COLOR_OFF);
   d.print(r.mx(),r.my(),medium,solid?COLOR_ON:COLOR_OFF,TextAlign::CENTER,level_label(n).c_str());
 }

 if(!s.notice.empty()) d.print(16,22,small,COLOR_OFF,TextAlign::CENTER_LEFT,s.notice.c_str());
}

// Small live hob symbol for the standby clock. Only drawn while watching.
template<class D> void draw_glance(D &d,esphome::display::BaseFont *small,esphome::display::BaseFont *medium,uint32_t now) {
 auto &s=ui;
 if(!s.watching) return;
 const bool fresh=s.fresh(now);
 const int gx=300,gy=372;
 const Rect cells[5]={{gx,gy+46,40,40},{gx,gy,40,40},{gx+50,gy+18,60,50},{gx+120,gy+46,40,40},{gx+120,gy,40,40}};
 for(int z=0;z<5;z++) {
   auto c=cells[z];
   const bool on=fresh && s.levels[z]>0;
   const bool hot=s.hot[z]=="Hot";
   if(on) {
     d.filled_rectangle(c.x,c.y,c.w,c.h,COLOR_OFF);
     d.print(c.mx(),c.my(),medium,COLOR_ON,TextAlign::CENTER,level_label(s.levels[z]).c_str());
   } else {
     d.rectangle(c.x,c.y,c.w,c.h,COLOR_OFF);
     if(hot) d.print(c.mx(),c.my(),small,COLOR_OFF,TextAlign::CENTER,"H");
   }
 }
 std::string text;
 if(!fresh) text="Hob status unknown";
 else if(int n=s.zones_on()) text=std::to_string(n)+(n==1?" zone on":" zones on");
 else if(s.on()) text="Hob on";
 else text="Residual heat";
 d.print(gx+180,gy+43,small,COLOR_OFF,TextAlign::CENTER_LEFT,text.c_str());
}
}

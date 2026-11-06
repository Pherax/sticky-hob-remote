#pragma once
namespace sticky {
struct Rect {
 int x,y,w,h;
 constexpr bool contains(int px,int py) const {return px>=x && px<x+w && py>=y && py<y+h;}
 constexpr int mx() const {return x+w/2;} constexpr int my() const {return y+h/2;}
};
constexpr Rect back_button{16,8,64,56}, power_button{364,32,72,72};
constexpr Rect left_zone{16,64,250,320};
constexpr Rect zone_button(int z){return z==2?Rect{282,112,236,200}:z<2?Rect{16,z==1?64:224,250,160}:Rect{534,z==4?64:232,250,152};}
inline bool rounded_contains(Rect r,int radius,int x,int y) {
 if(!r.contains(x,y))return false;
 int xx=x<r.x+radius?r.x+radius:x>=r.x+r.w-radius?r.x+r.w-radius-1:x;
 int yy=y<r.y+radius?r.y+radius:y>=r.y+r.h-radius?r.y+r.h-radius-1:y;
 return (x-xx)*(x-xx)+(y-yy)*(y-yy)<=radius*radius;
}
inline bool zone_contains(int z,int x,int y) {
 return z<2 ? rounded_contains(left_zone,40,x,y) && zone_button(z).contains(x,y)
            : rounded_contains(zone_button(z),36,x,y);
}
constexpr Rect feature_button(int i){return {292+i*76,320,64,64};}
constexpr Rect menu_button(int i){return {40+(i%3)*250,80+(i/3)*190,220,160};}
constexpr Rect level_button(int i){return {15+i*70,392,70,80};}
constexpr Rect timer_button(int i){return {40+i*250,340,220,80};}
constexpr Rect setting_button(int i){return {32,i==0?80:190+(i-1)*90,736,i==0?100:80};}
constexpr Rect connect_button(int i){return {40+i*380,120,340,55};}
constexpr Rect result_button(int i){return {40,190+i*55,720,50};}
constexpr Rect keypad_button(int i){return {40+(i%4)*180,150+(i/4)*60,172,54};}
constexpr int appliance_count(int page){return page==3?5:page==4?2:page==6?3:0;}
constexpr Rect appliance_button(int page,int i){return page==4?Rect{80+i*340,150,300,220}:page==6?Rect{40+i*250,150,220,220}:Rect{40+(i%3)*250,100+(i/3)*170,220,150};}
inline int appliance_at(int page,int x,int y){for(int i=0;i<appliance_count(page);i++)if(appliance_button(page,i).contains(x,y))return i;return -1;}
inline int menu_at(int x,int y){for(int i=0;i<6;i++)if(menu_button(i).contains(x,y))return i;return -1;}
inline int key_at(int x,int y){for(int i=0;i<15;i++)if(keypad_button(i).contains(x,y))return i;return -1;}
inline int level_at(int x,int y){for(int i=0;i<11;i++)if(level_button(i).contains(x,y))return i;return -1;}
}

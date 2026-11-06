#include <cassert>
#include <cmath>
#include <vector>
#include <string>
#include "../sticky/status_bar.h"
struct D {
 std::vector<std::string> glyphs; int rects=0;
 void rectangle(int x,int y,int w,int h,int){assert(x>=600&&x+w<=800&&y>=0&&y+h<60);rects++;}
 void filled_rectangle(int x,int y,int w,int h,int c){rectangle(x,y,w,h,c);}
 void print(int,int,esphome::display::BaseFont*,int,esphome::display::TextAlign,const char *s){glyphs.emplace_back(s);}
 template<class... A>void printf(A...){glyphs.emplace_back("%");}
};
int main(){
 auto &u=sticky::ui; u.controls=true; u.page=2; u.received=true;u.online=true;u.received_at=100;
 D d; sticky::draw_status(d,nullptr,nullptr,true,80,101);
 assert(d.glyphs.empty() && d.rects==0);                 // all good: nothing drawn
 D w; sticky::draw_status(w,nullptr,nullptr,false,80,101);
 assert(w.glyphs.size()==1 && w.glyphs[0]=="\U000F05AA"); // Wi-Fi lost
 D l; sticky::draw_status(l,nullptr,nullptr,true,80,9000);
 assert(l.glyphs.size()==1 && l.glyphs[0]=="\U000F0338"); // hob link stale
 D b; sticky::draw_status(b,nullptr,nullptr,true,12,101);
 assert(b.glyphs.size()==1 && b.rects>0);                 // low battery
 u.controls=false; D c; sticky::draw_status(c,nullptr,nullptr,false,80,9000);
 assert(c.glyphs.empty());                                // standby stays clean
}

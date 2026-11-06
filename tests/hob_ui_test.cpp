#include <cstdio>
#include <cassert>
#include "../sticky/hob_ui.h"
struct D {
 void filled_rectangle(int x,int y,int w,int h,int c){printf("F %d %d %d %d %d\n",x,y,w,h,c);}
 void draw_pixel_at(int x,int y,int){assert(x>=0&&y>=0&&x<800&&y<480);printf("P %d %d\n",x,y);}
 void rectangle(int x,int y,int w,int h,int){assert(x>=0&&y>=0&&x+w<=800&&y+h<=480);printf("R %d %d %d %d\n",x,y,w,h);}
 void line(int x,int y,int xx,int yy,int c){printf("L %d %d %d %d %d\n",x,y,xx,yy,c);}
 void circle(int x,int y,int r,int c){assert(x-r>=0&&y-r>=0&&x+r<800&&y+r<480);printf("C %d %d %d %d\n",x,y,r,c);}
 void filled_circle(int x,int y,int r,int c){circle(x,y,r,c);}
 void print(int x,int y,esphome::display::BaseFont *f,int,esphome::display::TextAlign,const char*s){printf("T %d %d %ld %s\n",x,y,(long)f,s);}
};
int main(){
 auto &s=sticky::ui;s.received=true;s.online=true;s.received_at=100;s.hob="On";s.levels={0,0,4,1,0};s.pans[2]="Present";s.hot[2]="Hot";
 s.req_zone=2;s.req_level=7;s.notice="Not applied - check hob";
 D d;sticky::draw_hob(d,(esphome::display::BaseFont*)96,(esphome::display::BaseFont*)48,(esphome::display::BaseFont*)28,(esphome::display::BaseFont*)18,(esphome::display::BaseFont*)49,(esphome::display::BaseFont*)29,101);
 s.watching=true;sticky::draw_glance(d,(esphome::display::BaseFont*)18,(esphome::display::BaseFont*)28,101);
}

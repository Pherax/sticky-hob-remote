#include <cstdio>
#include <cassert>
#include "../sticky/menu.h"
struct Display {
 void line(int x,int y,int xx,int yy,int){::printf("<path d='M%d %dL%d %d' stroke='black'/>\n",x,y,xx,yy);}
 void rectangle(int x,int y,int w,int h,int){::printf("<rect x='%d' y='%d' width='%d' height='%d' fill='none' stroke='black'/>\n",x,y,w,h);}
 void circle(int x,int y,int r,int){::printf("<circle cx='%d' cy='%d' r='%d' fill='none' stroke='black'/>\n",x,y,r);}
 void filled_circle(int,int,int,int){}
 template<typename... A> void print(A...){ }
 template<typename... A> void printf(A...){ }
};
int main() {
 assert(sticky::menu_at(140,165)==0); assert(sticky::menu_at(400,165)==1);
 assert(sticky::menu_at(660,165)==2); assert(sticky::menu_at(140,365)==3);
 assert(sticky::menu_at(400,365)==4); assert(sticky::menu_at(660,365)==5);
 assert(sticky::menu_at(10,20)==-1);assert(sticky::menu_at(800,480)==-1);
 for(int page:{3,4,6}) {
   for(int i=0;i<sticky::appliance_count(page);i++) {
     auto r=sticky::appliance_button(page,i);
     assert(sticky::appliance_at(page,r.x,r.y)==i);
     assert(sticky::appliance_at(page,r.x+r.w-1,r.y+r.h-1)==i);
     assert(sticky::appliance_at(page,r.x+r.w,r.y)==-1);
   }
 }
 Display d; sticky::ui.open(100);
 puts("<svg xmlns='http://www.w3.org/2000/svg' width='800' height='480' viewBox='0 0 800 480'><rect width='800' height='480' fill='white'/>");
 sticky::draw_page(d,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,100,false,80);
 puts("</svg>");
}

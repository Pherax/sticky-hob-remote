#include <cassert>
#include "../sticky/layout.h"
int main(){
 using namespace sticky;
 for(int y=0;y<480;y++)for(int x=0;x<800;x++){
  int count=0,want=-1;
  for(int i=0;i<6;i++)if(menu_button(i).contains(x,y)){count++;want=i;}
  assert(count<=1&&menu_at(x,y)==want);
  count=0;want=-1;
  for(int i=0;i<15;i++)if(keypad_button(i).contains(x,y)){count++;want=i;}
  assert(count<=1&&key_at(x,y)==want);
 }
 assert(key_at(215,175)==-1); // visible keypad gap
 assert(key_at(700,360)==-1); // unused bottom-right key
 assert(menu_at(275,160)==-1); // visible menu gap
 assert(level_at(139,380)==-1);
 assert(!power_button.contains(790,30));
 for(int y=0;y<480;y++)for(int x=0;x<800;x++){
  int count=power_button.contains(x,y);
  for(int z=0;z<5;z++)count+=zone_contains(z,x,y);
  for(int n=0;n<11;n++)count+=level_button(n).contains(x,y);
  for(int i=0;i<3;i++)count+=feature_button(i).contains(x,y);
  assert(count<=1);
 }
 assert(!zone_contains(1,16,64));assert(zone_contains(1,142,64));
 assert(zone_contains(1,142,223));assert(!zone_contains(1,142,224));assert(zone_contains(0,142,224));
 assert(!zone_contains(2,282,112));assert(zone_contains(2,400,112));
 for(int n=0;n<11;n++){auto r=level_button(n);assert(level_at(r.x,r.y)==n);assert(level_at(r.x+r.w-1,r.y+r.h-1)==n);assert(level_at(r.x+r.w,r.y)==(n<10?n+1:-1));}
 for(int i=0;i<6;i++){auto r=menu_button(i);assert(menu_at(r.mx(),r.my())==i);}
}

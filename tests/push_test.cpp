#include "../sticky/push.h"
#include <cassert>
// Encode exactly like hob/sticky-direct.yaml and check the round trip.
static float levels(const int l[5]){uint32_t v=0;for(int z=0;z<5;z++)v|=uint32_t(l[z]+1)<<(4*z);return float(v);}
static float flags(const int p[5],const int h[5]){uint32_t v=0;for(int z=0;z<5;z++){v|=uint32_t(p[z]+1)<<(2*z);v|=uint32_t(h[z]+1)<<(10+2*z);}return float(v);}
static float meta(int mode,int state,bool master,bool ready,bool fault){return float(uint32_t(mode)|uint32_t(state)<<3|uint32_t(master)<<5|uint32_t(ready)<<6|uint32_t(fault)<<7);}
int main(){
 auto &s=sticky::ui;
 int l[5]={0,-1,6,10,3}, p[5]={1,-1,0,1,0}, h[5]={0,1,-1,1,0};
 sticky::push_value(0,levels(l),100);
 sticky::push_value(1,flags(p,h),100);
 sticky::push_value(2,meta(4,2,true,true,false),100);
 assert(!s.received);                       // nothing applied before all four values arrive
 sticky::push_value(3,42,100);
 assert(s.received && s.fresh(100));
 for(int z=0;z<5;z++) assert(s.levels[z]==l[z]);
 assert(s.levels[3]==10 && s.pans[0]=="Present" && s.pans[1]=="Unknown" && s.hot[1]=="Hot" && s.hot[2]=="Unknown");
 assert(s.mode=="Master" && s.hob=="Commanded on" && s.ready && !s.fault);
 // same tick does not renew freshness
 sticky::push_value(3,42,7000); assert(!s.fresh(9000));
 sticky::push_value(3,43,9000); assert(s.fresh(9000));
 // change-only packet applies at once without renewing freshness
 l[2]=4; sticky::push_value(0,levels(l),9100); assert(s.levels[2]==4);
 // garbage is ignored
 sticky::push_value(2,meta(7,0,false,false,false),9200); assert(s.mode=="Master");
 sticky::push_value(0,-1,9200); sticky::push_value(0,NAN,9200); assert(s.levels[2]==4);
 // Dev observing the original panel
 sticky::push_value(2,meta(3,1,false,false,true),9300); assert(s.mode=="Dev" && s.hob=="Off" && s.fault);
 assert(sticky::push_recent(9300) && !sticky::push_recent(14000));
 // the largest packed value stays exact in a float
 int lmax[5]={10,10,10,10,10}; assert(uint32_t(levels(lmax))==0xBBBBBu);
}

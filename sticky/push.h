#pragma once
#include <cmath>
#include <cstdint>
#include "ui_state.h"
namespace sticky {
// Decodes the four integer-packed values the hob pushes over encrypted UDP
// (see hob/sticky-direct.yaml). Index: 0 levels, 1 flags, 2 meta, 3 tick.
struct Push {
 uint32_t raw[4]={0,0,0,0};
 uint8_t seen=0;           // bit per value received since boot
 uint32_t tick=0;          // last hob tick (seconds) that renewed freshness
 bool tick_valid=false;
 uint32_t last_at=0;       // millis() of the last packet of any kind
 bool any() const { return last_at!=0; }
};
static Push push;
inline bool decode_push(const Push &p,std::string &mode,std::string &hob,bool &ready,bool &fault,
                        int levels[5],int pan[5],int hot[5]) {
 if(p.seen!=0x0F) return false;
 for(int z=0;z<5;z++) {
   int l=int((p.raw[0]>>(4*z))&0xF)-1;
   if(l>10) return false;
   levels[z]=l;
   int pv=int((p.raw[1]>>(2*z))&0x3)-1, hv=int((p.raw[1]>>(10+2*z))&0x3)-1;
   if(pv>1||hv>1) return false; pan[z]=pv; hot[z]=hv;
 }
 static const char *modes[]={"Unknown","Auto","Listen","Dev","Master"};
 uint32_t m=p.raw[2]&0x7; if(m>4) return false; mode=modes[m];
 uint32_t st=(p.raw[2]>>3)&0x3; bool master=(p.raw[2]>>5)&1;
 hob=st==0?"Unknown":st==2?(master?"Commanded on":"On"):(master?"Commanded off":"Off");
 ready=(p.raw[2]>>6)&1; fault=(p.raw[2]>>7)&1;
 return true;
}
// Called from each remote sensor's on_value.
inline void push_value(int index,float value,uint32_t now) {
 if(index<0||index>3||!std::isfinite(value)||value<0||value>16777215.0f) return;
 auto &p=push; p.raw[index]=uint32_t(value); p.seen|=1u<<index; p.last_at=now;
 bool live=false;
 if(index==3) { live=!p.tick_valid || p.raw[3]!=p.tick; p.tick=p.raw[3]; p.tick_valid=true; }
 std::string mode,hob; bool ready,fault; int levels[5],pan[5],hot[5];
 if(!decode_push(p,mode,hob,ready,fault,levels,pan,hot)) return;
 apply_snapshot(ui,mode,hob,ready,fault,levels,pan,hot,live,now);
}
// Push is the primary source; HTTP polling only runs when no push arrived recently.
inline bool push_recent(uint32_t now) { return push.any() && uint32_t(now-push.last_at)<4000; }
}

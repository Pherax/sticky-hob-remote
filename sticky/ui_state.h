#pragma once
#include "layout.h"
#include <array>
#include <string>
#include <cstdint>
namespace sticky {
struct State {
 uint32_t next_poll=0;
 bool ready=false, fault=false; uint32_t source_tick=0;
 int page=0; // 0 clock, 1 menu, 2 hob, 3 hood, 4 coffee, 5 timer, 6 lights, 7 settings, 8 connection, 9 keypad
 uint32_t timer_end=0; int timer_minutes=5; bool timer_running=false, timer_done=false;
 bool controls=false, dirty=true, received=false;
 int selected=2;
 // draft: level waiting to be sent for the selected zone (-1 none). The send pump picks it up.
 int draft=-1;
 // Requested (not yet reported) level: drawn as an outline until the hob reports it.
 int req_zone=-1, req_level=-1; uint32_t req_at=0;
 bool pending_on=false; uint32_t on_req_at=0;
 // Standby glance: Wi-Fi stays on in standby while the hob was last seen active.
 bool watching=false;
 uint32_t activity=0, received_at=0, painted_at=0, maintenance_until=0;
 double snapshot_ts=0;
 std::string ha_notice;
 std::string mode="Unknown", hob="Unknown", notice;
 std::array<int,5> levels{{-1,-1,-1,-1,-1}};
 std::array<std::string,5> pans{{"Unknown","Unknown","Unknown","Unknown","Unknown"}};
 std::array<std::string,5> hot{{"Unknown","Unknown","Unknown","Unknown","Unknown"}};
 bool online=false;
 bool fresh(uint32_t now) const { return received && online && uint32_t(now-received_at)<8000; }
 bool writable(uint32_t now) const { return fresh(now) && (mode=="Master" || mode=="Dev"); }
 bool on() const { return hob=="On" || hob=="Commanded on"; }
 bool heating_allowed(uint32_t now) const { return writable(now) && on(); }
 bool maintenance(uint32_t now) const { return maintenance_until && int32_t(maintenance_until-now)>0; }
 // Anything that still matters when walking away: hob on, a zone set, or residual heat.
 bool active() const {
   if(on()) return true;
   for(int i=0;i<5;i++) if(levels[i]>0 || hot[i]=="Hot") return true;
   return false;
 }
 int zones_on() const { int n=0; for(int i=0;i<5;i++) if(levels[i]>0) n++; return n; }
 void clear_requests() { draft=-1; req_zone=-1; req_level=-1; pending_on=false; }
 void open(uint32_t now) { controls=true; page=1; activity=now; clear_requests(); dirty=true; notice.clear(); ha_notice.clear(); }
 void clock() { controls=false; page=0; clear_requests(); dirty=true; }
 void navigate(int target, uint32_t now) { page=target; controls=target!=0; activity=now; clear_requests(); dirty=true; notice.clear(); ha_notice.clear(); }
 void start_timer(uint32_t now) { timer_end=now+uint32_t(timer_minutes)*60000; timer_running=true; timer_done=false; dirty=true; }
 bool tick_timer(uint32_t now) { if(timer_running && int32_t(now-timer_end)>=0) {timer_running=false; timer_done=true; dirty=true; return true;} return false; }
 uint32_t remaining(uint32_t now) const { return timer_running && int32_t(timer_end-now)>0 ? (timer_end-now+999)/1000 : 0; }
 // Values here are requested settings; they never overwrite received levels.
 void select(int zone) { if(zone>=0 && zone<5 && zone!=selected) { selected=zone; draft=-1; pending_on=false; dirty=true; } }
 bool can_apply(uint32_t now) const {
   return writable(now) && (draft==0 || ((!fault && (mode!="Master" || ready)) && (pending_on || (draft>=1 && draft<=10 && on()))));
 }
 // Ask for a level on the selected zone. Returns false (with a notice) when it cannot be sent.
 bool request_level(int level, uint32_t now) {
   level=level<0?0:level>10?10:level;
   if(!writable(now)) { notice="Hob not connected"; dirty=true; return false; }
   if(level>0 && !on()) { notice="Switch the hob on first"; dirty=true; return false; }
   if(level>0 && (fault || (mode=="Master" && !ready))) { notice=fault?"Hob fault":"Hob not ready"; dirty=true; return false; }
   draft=level; req_zone=selected; req_level=level; req_at=now; pending_on=false; notice.clear(); dirty=true;
   return true;
 }
 // UP/DOWN keys step from the pending request if there is one, else from the reported level.
 bool step_level(int delta, uint32_t now) {
   int base=(req_zone==selected && req_level>=0) ? req_level : levels[selected];
   if(base<0) { notice="Hob not connected"; dirty=true; return false; }
   int target=base+delta;
   if(target<0 || target>10) return false;
   return request_level(target, now);
 }
 // Drop requests the hob confirmed, or that it did not take within 4 s.
 void settle_requests(uint32_t now) {
   if(req_zone>=0) {
     if(fresh(now) && levels[req_zone]==req_level && draft<0) { req_zone=-1; req_level=-1; dirty=true; }
     else if(uint32_t(now-req_at)>4000 && draft<0) { req_zone=-1; req_level=-1; notice="Not applied - check hob"; dirty=true; }
   }
   if(pending_on && uint32_t(now-on_req_at)>4000) { pending_on=false; dirty=true; }
   if(pending_on && on()) { pending_on=false; dirty=true; }
 }
};
static State ui;
static const char *names[5]={"Bottom left","Top left","Middle","Bottom right","Top right"};
inline int zone_at(int x,int y) {
 for(int z=0;z<5;z++) if(zone_contains(z,x,y)) return z;
 return -1;
}
inline const char *pan_name(int v){return v<0?"Unknown":v?"Present":"Absent";}
inline const char *hot_name(int v){return v<0?"Unknown":v?"Hot":"Not hot";}
// One place where hob state enters the UI, from the UDP push or the HTTP snapshot.
// `live` = the source proved the data is new (its tick advanced); only then does freshness renew.
inline void apply_snapshot(State &s,const std::string &mode,const std::string &hob,bool ready,bool fault,
                           const int levels[5],const int pan[5],const int hot[5],bool live,uint32_t now) {
 std::string old=s.mode+s.hob+std::to_string(s.ready)+std::to_string(s.fault);
 for(int i=0;i<5;i++) old+=std::to_string(s.levels[i])+s.pans[i]+s.hot[i];
 s.mode=mode; s.hob=hob; s.ready=ready; s.fault=fault;
 for(int i=0;i<5;i++) { s.levels[i]=levels[i]; s.pans[i]=pan_name(pan[i]); s.hot[i]=hot_name(hot[i]); }
 std::string updated=s.mode+s.hob+std::to_string(s.ready)+std::to_string(s.fault);
 for(int i=0;i<5;i++) updated+=std::to_string(s.levels[i])+s.pans[i]+s.hot[i];
 const bool was_fresh=s.fresh(now);
 if(live || s.received) { if(live) s.received_at=now; s.received=true; s.online=true; }
 if(old!=updated || was_fresh!=s.fresh(now)) s.dirty=true;
 s.settle_requests(now);
}
// Only inputs actually drawn on the hob page. Ignore counters, readiness and notices.
inline std::string hob_visual_key(const State &s,uint32_t now,bool wifi,int battery) {
 const bool fresh=s.fresh(now);
 std::string key=std::to_string(s.selected)+":"+std::to_string(fresh)+":"+std::to_string(fresh && s.on())+":"+std::to_string(wifi)+":"+std::to_string(battery<20?battery:100);
 key+=":"+std::to_string(s.req_zone)+":"+std::to_string(s.req_level)+":"+std::to_string(s.pending_on)+":"+s.notice;
 for(int i=0;i<5;i++)key+=":"+std::to_string(fresh?s.levels[i]:-1)+":"+std::to_string(fresh && s.pans[i]=="Present")+":"+std::to_string(s.hot[i]=="Hot");
 return key;
}
// What the standby clock shows about the hob.
inline std::string glance_key(const State &s,uint32_t now) {
 if(!s.watching) return "";
 std::string key=std::to_string(s.fresh(now));
 for(int i=0;i<5;i++) key+=":"+std::to_string(s.levels[i])+":"+s.hot[i];
 return key;
}
inline std::string level_label(int level) { return level<0?"?":level==10?"P":std::to_string(level); }
// A tap that came in while the e-paper was refreshing; handled when it is done.
struct PendingTap { int x=0, y=0; uint32_t at=0; };
inline PendingTap pending_tap{};
inline uint32_t last_tap=0;
}

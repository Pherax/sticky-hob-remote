#include "../sticky/ui_state.h"
#include <cassert>
int main() {
 sticky::State s;
 s.open(100); assert(!s.can_apply(100));
 s.ready=true; s.online=true; s.received=true; s.received_at=100; s.mode="Master"; s.hob="Commanded off";
 s.draft=3; assert(!s.can_apply(101));
 s.pending_on=true; assert(s.can_apply(101));
 s.pending_on=false; s.hob="Commanded on"; assert(s.can_apply(101));
 assert(!s.can_apply(15100));
 s.mode="Listen"; assert(!s.can_apply(101));
 s.mode="Dev"; assert(s.can_apply(101));
 s.draft=11; assert(!s.can_apply(101));
 s.draft=0; s.hob="Off"; assert(s.can_apply(101));
 s.select(3); assert(s.draft==-1 && s.selected==3);
 assert(sticky::zone_at(665,265)==3);
 assert(sticky::zone_at(135,125)==1);
 assert(sticky::zone_at(400,195)==2);
 s.received_at=0xfffffff0; assert(s.fresh(10)); // millis wrap
 s.clock(); assert(!s.controls && s.draft==-1);
}

struct TimerChecks {
 TimerChecks() {
  sticky::State s;
  s.open(100); assert(s.page==1 && s.controls);
  s.draft=9; s.pending_on=true;
  s.navigate(2,200); assert(s.page==2 && s.draft==-1 && !s.pending_on);
  s.timer_minutes=1; s.start_timer(0xfffffff0);
  assert(s.remaining(0xfffffff0)==60);
  assert(!s.tick_timer(500));
  assert(s.tick_timer(uint32_t(0xfffffff0+60000)));
  assert(!s.timer_running && s.timer_done);
  assert(!s.tick_timer(70000));
  s.clock(); assert(s.page==0 && !s.controls);
 }
} timer_checks;

struct DirectSafetyChecks {
 DirectSafetyChecks() {
  sticky::State s; s.open(10); s.navigate(2,10);
  s.received=true; s.online=true; s.received_at=10;
  s.mode="Master"; s.hob="Commanded on"; s.draft=1;
  assert(!s.can_apply(11)); // Master not ready
  s.ready=true; assert(s.can_apply(11));
  s.fault=true; assert(!s.can_apply(11));
  s.draft=0; assert(s.can_apply(11)); // allow an explicit zero
  s.fault=false; s.draft=1; assert(!s.can_apply(8010));
  s.received=false; assert(!s.can_apply(11));
  s.received=true; s.mode="Auto"; assert(!s.can_apply(11));
  s.mode="Listen"; assert(!s.can_apply(11));
 }
} direct_safety_checks;

struct VisualKeyChecks {
 VisualKeyChecks(){
  sticky::State s;s.received=true;s.online=true;s.received_at=100;
  auto key=sticky::hob_visual_key(s,101,true,80);
  s.source_tick++;s.ready=true;
  assert(key==sticky::hob_visual_key(s,102,true,80));
  s.notice="Switch the hob on first"; // notices are drawn now
  assert(key!=sticky::hob_visual_key(s,102,true,80)); s.notice.clear();
  s.selected=3;assert(key!=sticky::hob_visual_key(s,102,true,80));
  s.selected=2;s.pans[0]="Present";
  assert(key!=sticky::hob_visual_key(s,102,true,80));
 }
} visual_key_checks;

// v0.13: requested levels, UP/DOWN steps, settling.
struct RequestChecks {
 RequestChecks(){
  sticky::State s; s.open(10); s.navigate(2,10);
  s.received=true; s.online=true; s.received_at=10; s.mode="Master"; s.ready=true;
  s.hob="Commanded off"; s.levels={0,0,0,0,0}; s.selected=2;
  assert(!s.request_level(3,11) && s.notice=="Switch the hob on first" && s.draft==-1);
  assert(s.request_level(0,11) && s.draft==0 && s.req_level==0); s.clear_requests();
  s.hob="Commanded on";
  assert(s.step_level(+1,11) && s.draft==1 && s.req_zone==2 && s.req_level==1);
  // a second press before the hob reports steps from the pending request
  assert(s.step_level(+1,12) && s.draft==2 && s.req_level==2);
  assert(!s.step_level(-1,12) || s.req_level==1);
  s.draft=-1; s.req_level=2; // pretend delivered
  s.levels[2]=2; s.settle_requests(13); assert(s.req_zone==-1 && s.notice.empty());
  // not applied within 4 s -> notice
  assert(s.request_level(5,20)); s.draft=-1; s.settle_requests(4100); assert(s.req_zone==-1 && s.notice=="Not applied - check hob");
  // limits
  s.levels[2]=10; s.notice.clear(); assert(!s.step_level(+1,4200)); s.levels[2]=0; assert(!s.step_level(-1,4200));
  // fault blocks heating but not zero
  s.fault=true; assert(!s.request_level(4,4200)); assert(s.request_level(0,4200));
  // stale -> nothing
  sticky::State t; assert(!t.step_level(+1,5));
 }
} request_checks;

// Standby glance: active means on, a zone set, or residual heat.
struct ActiveChecks {
 ActiveChecks(){
  sticky::State s; s.hob="Commanded off"; s.levels={0,0,0,0,0};
  assert(!s.active()); s.hot[3]="Hot"; assert(s.active()); s.hot[3]="Not hot";
  s.levels[1]=4; assert(s.active() && s.zones_on()==1);
  s.watching=true; s.received=true; s.online=true; s.received_at=5;
  auto k=sticky::glance_key(s,6); s.levels[1]=5; assert(k!=sticky::glance_key(s,6));
 }
} active_checks;

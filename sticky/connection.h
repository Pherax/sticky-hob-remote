#pragma once
#include "connection_state.h"
#include "ui_state.h"
#include "mdns.h"
#include "esphome/core/preferences.h"
#include <cstring>
namespace sticky {
struct SavedConnection {char url[101];};
static esphome::ESPPreferenceObject connection_pref;
inline void load_connection(const char *fallback) {
 connection_pref=esphome::global_preferences->make_preference<SavedConnection>(0x57C04B02);
 SavedConnection saved{};
 if(connection_pref.load(&saved)&&memchr(saved.url,0,sizeof(saved.url))&&safe_address(saved.url)) connection.address=saved.url;
 else connection.address=safe_address(fallback)?fallback:"http://induction.local";
}
inline bool save_connection() {
 if(!safe_address(connection.candidate))return false;
 SavedConnection saved{};strncpy(saved.url,connection.candidate.c_str(),sizeof(saved.url)-1);
 if(!connection_pref.save(&saved)||!esphome::global_preferences->sync())return false;
 connection.address=connection.candidate;ui.next_poll=0;
 ui.received=false;ui.online=false;ui.draft=-1;ui.pending_on=false;
 ui.levels.fill(-1);ui.pans.fill("Unknown");ui.hot.fill("Unknown");
 ui.ready=false;ui.fault=false;ui.hob="Unknown";ui.mode="Unknown";
 return true;
}
inline void discover_hobs() {
 auto &c=connection;c.found.clear();mdns_result_t *results=nullptr;
 // Explicit user action, bounded 1.5s query; no subnet scan or command traffic.
 auto error=mdns_query_ptr("_macshob","_tcp",1500,8,&results);
 if(error==ESP_OK)for(auto *r=results;r;r=r->next) {
  if(!r->hostname||!r->port)continue;
  std::string url=std::string("http://")+r->hostname+".local";
  if(r->port!=80)url+=":"+std::to_string(r->port);
  if(safe_address(url)&&std::find(c.found.begin(),c.found.end(),url)==c.found.end()&&c.found.size()<4)c.found.push_back(url);
 }
 if(results)mdns_query_results_free(results);
 c.scanning=false;c.message=c.found.empty()?"No hobs found. Rescan or enter IP.":"Select a hob to test and save.";ui.dirty=true;
}
}

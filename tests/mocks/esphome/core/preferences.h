#pragma once
#include <cstring>
namespace esphome {
inline unsigned char saved_data[101]{};
inline bool have_saved=false,write_ok=true,sync_ok=true;
struct ESPPreferenceObject {
 template<class T> bool load(T *p){if(!have_saved)return false;memcpy(p,saved_data,sizeof(T));return true;}
 template<class T> bool save(const T *p){if(!write_ok)return false;memcpy(saved_data,p,sizeof(T));have_saved=true;return true;}
};
struct Preferences {template<class T> ESPPreferenceObject make_preference(unsigned){return {};};bool sync(){return sync_ok;}};
inline Preferences prefs;
inline Preferences *global_preferences=&prefs;
}

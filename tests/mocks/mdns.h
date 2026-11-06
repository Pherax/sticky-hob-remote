#pragma once
#include <cstdint>
constexpr int ESP_OK=0;
struct mdns_result_t {mdns_result_t *next=nullptr;char *hostname=nullptr;uint16_t port=80;};
inline mdns_result_t *mock_results=nullptr;
inline bool freed=false;
inline int mdns_query_ptr(const char*,const char*,uint32_t timeout,size_t maximum,mdns_result_t **r){assert(timeout<=2000&&maximum<=8);*r=mock_results;return 0;}
inline void mdns_query_results_free(mdns_result_t*){freed=true;}

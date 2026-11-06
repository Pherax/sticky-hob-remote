#include <cassert>
#include "../sticky/connection.h"
int main(){
 using namespace sticky;
 assert(valid_ip("192.168.1.50"));assert(valid_ip("192.168.4.1:8080"));
 for(auto s:{"192.168.1.256","192.168.1.","192.168.01.1","1.2.3","1.2.3.4:0","1.2.3.4:65536","1.2.3.4:x","1.2.3.4/path","224.0.0.1"})assert(!valid_ip(s));
 assert(!safe_address("http://host/path"));assert(!safe_address("http://a@b"));
 load_connection("http://induction.local");assert(connection.address=="http://induction.local");
 connection.candidate="http://192.168.1.50";ui.draft=9;ui.pending_on=true;ui.received=true;ui.hot[0]="Hot";
 assert(save_connection());assert(ui.draft==-1&&!ui.pending_on&&!ui.received&&ui.hot[0]=="Unknown");
 connection.address.clear();load_connection("http://other.local");assert(connection.address=="http://192.168.1.50");
 esphome::write_ok=false;connection.candidate="http://192.168.1.51";assert(!save_connection());assert(connection.address=="http://192.168.1.50");
 char host[]="hob",bad[]="bad/path";mdns_result_t c{nullptr,bad,80},b{&c,host,80},a{&b,host,80};mock_results=&a;
 discover_hobs();assert(freed&&connection.found.size()==1&&connection.found[0]=="http://hob.local");
 assert(key_at(40,150)==0&&key_at(400,360)==14&&key_at(10,200)==-1);
}

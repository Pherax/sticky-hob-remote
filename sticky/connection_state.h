#pragma once
#include <string>
#include "layout.h"
#include <vector>
#include <algorithm>
namespace sticky {
struct Connection {
 std::string address, candidate, input, message;
 std::vector<std::string> found;
 bool scanning=false, testing=false;
 bool busy() const {return scanning||testing;}
};
static Connection connection;
// Manual entry is deliberately IPv4 only, optionally followed by :port.
inline bool valid_ip(const std::string &s) {
 auto colon=s.find(':'); std::string ip=s.substr(0,colon);
 if(colon!=std::string::npos) {
  std::string port=s.substr(colon+1); if(port.empty()||port.size()>5)return false;
  unsigned n=0;for(char c:port){if(c<'0'||c>'9')return false;n=n*10+c-'0';} if(n==0||n>65535)return false;
 }
 int count=0;size_t start=0;
 while(start<ip.size()) {
  auto end=ip.find('.',start);if(end==std::string::npos)end=ip.size();
  auto part=ip.substr(start,end-start);if(part.empty()||part.size()>3||(part.size()>1&&part[0]=='0'))return false;
  int n=0;for(char c:part){if(c<'0'||c>'9')return false;n=n*10+c-'0';}if(n>255)return false;
  if(count==0&&(n==0||n>=224))return false;
  count++;start=end+1;
 }
 return count==4&&!ip.empty()&&ip.back()!='.';
}
inline bool safe_address(const std::string &s) {
 if(s.size()<8||s.size()>100||s.compare(0,7,"http://")!=0)return false;
 for(size_t i=7;i<s.size();i++){char c=s[i];if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='.'||c=='-'||c==':'))return false;}
 return true;
}
inline std::string short_address(const std::string &s){return s.size()>64?s.substr(0,61)+"...":s;}
static const char *keys[16]={"1","2","3","4","5","6","7","8","9",".","0",":","Clear","<","Test",""};

}

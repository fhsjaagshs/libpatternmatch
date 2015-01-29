#include <iostream>
#include "pattern.h"

int main() {
//  pattern wildcard("users.<format>.<compression>");
  pattern splat("users.*.*");
 // pattern mixed("users.<format>.*.*");
  
  list<string> splats = splat.match_splats("users.json.tar");
  
  for (list<string>::iterator i = splats.begin(); i != splats.end(); ++i) {
    cout << *i << endl;
  }
  
  return 0;
}
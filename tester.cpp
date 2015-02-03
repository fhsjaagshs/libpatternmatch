#include <iostream>
#include "pattern.h"

using namespace std;

void test_pattern(char *ptrn, char *test) {
  LibPM::pattern p(ptrn);
  bool matches = p.matches(test);
  //bool matches = p.match(test,splats,wildcards);
  cout << "  Pattern: " << ptrn << endl;
  cout << "     Test: " << test << endl;
  cout << "  Matches: " << (matches ? "yes" : "no");
  
  if (matches) {
    list<string> splats = p.match_splats(test);
    if (splats.size() > 0) {
      cout << endl;
      cout << "   Splats: " << splats.size() << " (";
  
      for (list<string>::iterator i = splats.begin(); i != splats.end(); i++) {
        cout << *i;
        if (next(i) != splats.end()) cout << ", ";
      }
  
      cout << ")";
    }
    
    map<string, string> wildcards = p.match_wildcards(test);
    
    if (wildcards.size() > 0) {
      cout << endl;
      cout << "Wildcards: " << wildcards.size() << " (";

      for (map<string, string>::iterator i = wildcards.begin(); i != wildcards.end(); i++) {
        cout << i->first << ":" << i->second;
        if (next(i) != wildcards.end()) cout << ", ";
      }
  
      cout << ")";
    }
  }
  
  cout << endl;
}

int main(int argc, char* argv[]) {
  if (argc == 3) test_pattern(argv[1], argv[2]);
  else cout << "No pattern to test" << endl;
  return 0;
}

#include <iostream>
#include "pattern.h"

using namespace std;

void test_pattern(char *ptrn, char *test) {
  LibPM::pattern p(ptrn);
  bool matches = p.matches(test);
  list<string> splats = p.match_splats(test);
  cout << "  Pattern: " << ptrn << endl;
  cout << "     Test: " << test << endl;
  cout << "  Matches: " << (matches ? "yes" : "no") << endl;
  cout << "Wildcards: " << splats.size() << " (";
  
  for (list<string>::iterator i = splats.begin(); i != splats.end();) {
    cout << *i;
    if (++i != splats.end()) cout << ", ";
  }
  
  cout << ")" << endl;
}

int main(int argc, char* argv[]) {
  if (argc == 3) test_pattern(argv[1], argv[2]);
  else cout << "No pattern to test" << endl;
  return 0;
}
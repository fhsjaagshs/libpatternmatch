#include <iostream>
#include "pattern.h"

using namespace std;

void test_pattern(char *ptrn, char *test) {
  LibPM::pattern p(ptrn);
  bool matches = p.matches(test);
 // pattern mixed("users.<format>.*.*");
  cout << "Pattern: " << ptrn << endl;
  cout << "   Test: " << test << endl;
  cout << "Matches: " << (matches ? "yes" : "no") << endl << endl;
  
  // list<string> splats = splat.match_splats("users.json.tar");
//
//   for (list<string>::iterator i = splats.begin(); i != splats.end(); ++i) {
//     cout << *i << endl;
//   }
}

int main() {
 // test_pattern(":value", "this");
  test_pattern("users.*..*", "users.json..lzma");
//  test_pattern("<resource>.<json>", "users.json");
  return 0;
}
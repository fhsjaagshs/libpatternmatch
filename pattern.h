#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <iostream>
#include <list>
#include <map>

namespace LibPM {
  using namespace std;
  
  class pattern {
  public:
    pattern(const char *ptrn) { create(ptrn); }
    pattern(string ptrn) { create(ptrn); }
    
    bool operator==(const string &rhs) const;
    bool operator!=(const string &rhs) const;
  
    // returns true if there are any wilcards/splats/etc in this
    bool is_dynamic() const;
    
    // check if a string matches the this pattern
    bool matches(string comp) const;
    
    // extracts all mappings from cppstr
    map<string, string> match_wildcards(string cppstr) const;
    
    // extracts all splats from cppstr
    list<string> match_splats(string cppstr) const;
  protected:
    string _pattern;
    map<unsigned, string> _index;
    
    void create(string ptrn);
    
    // advance ptr to the next occurance of str
    unsigned _advance_to_str(char *&ptr, char *str, unsigned len) const;
    
    map <unsigned, string> _gen_indeces(char *str, char *ptr) const;
  };
}

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
    pattern(const char *ptrn) { create(string(ptrn)); }
    pattern(string ptrn) { create(ptrn); }
  
    bool is_dynamic();
    bool matches(string comp);
    map<string, string> match_wildcards(string comp);
    list<string> match_splats(string comp);
  
  private:
    string _pattern;
    list<unsigned> _splat_indeces;
    list<unsigned> _wildcard_indeces;
  
    // "show.<format>.<compression>"
    // "show.json.tar.gz"
    //
    // "foo.*.baz"
    // "foo.bar.baz"
  
    void create(string ptrn);
    
    // advance ptr to the first occurance of str
    unsigned _advance_to_str(char *&ptr, char *str, unsigned len);

    // advance to first char of wildcard sequence
    unsigned _advance_to_wildcard(char *&ptr, char c = '\0');
  
    // advance ptr to the first char after a wildcard sequence
    unsigned _advance_to_succ(char *&ptr);
  
    // compares a string to a pattern
    bool _ptrncmp(char *pattern, char *comp);
  
    list<string> _get_splats(char *pattern, char *comp);
    map<string, string> _get_wildcards(char *pattern, char *comp);
    list<unsigned> _get_splat_indeces(char *str, char *ptr=NULL);
    list<unsigned> _get_wildcard_indeces(char *str, char *ptr=NULL);
  
    string _read_until_wildcard(char *ptr);
    string _read_until(char *ptr, char c);
  };
}

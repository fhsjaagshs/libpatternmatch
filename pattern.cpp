#include "pattern.h"

// TODO: Match last occurance of the 'succ' string when reading wildcards
// TODO: New wildcard: & matches 1 character

namespace LibPM {
  bool pattern::operator==(const string &rhs) const {
    return matches(rhs);
  }
  
  bool pattern::operator!=(const string &rhs) const {
    return !(*this == rhs);
  }
  
  bool pattern::is_dynamic() const {
    return (_index.size() > 0);
  }

  bool pattern::matches(string comp) const {
    if (_pattern[0] == ':') return true;
    if (_pattern == comp) return true;
    return _ptrncmp((char *)_pattern.c_str(), (char *)comp.c_str());
  }
  
  /**/
  
  // all-in-one determine if a string matches while extracting the splats & wildcards
  // Advantages:
  //  One iteration over input string & pattern
  // Disadvantages:
  //  Cannot cache location of splats & wildcards, requiring much more processing
  // bool pattern::match(string cppstr, list<string> &splats, map<string,string> &wildcards) {
  //   if (_pattern[0] == ':') {
  //     map<string, string> m;
  //     m[_pattern.substr(1, _pattern.length()-1)] = cppstr;
  //     return true;
  //   }
  //
  //   char *str = (char *)cppstr.c_str();
  //   char *pattern = (char *)_pattern.c_str();
  //
  //   while (*pattern != '\0') {
  //     _advance_to_wildcard(pattern);
  //     if (*pattern == '*') {
  //
  //     }
  //   }
  //
  // }
  
   map<string, string> pattern::match_wildcards(string cppstr) const  {
     map<string,string> wildcards;
     char *pattern = (char *)_pattern.c_str();
     char *str = (char *)cppstr.c_str();
     int index_delta = 0;

     for (auto i = _index.begin(); i != _index.end(); i++) {
       unsigned idx = i->first;
       string token = i->second;
      
       char *succ = pattern+idx+token.length();
       char *vptr = str+idx+index_delta;
       char *tmpv = vptr;
       unsigned succlen = (i == _index.end()) ? 0 : next(i)->first-(idx+token.length());
       unsigned vlen = _advance_to_str(tmpv, succ, succlen);
      
       index_delta += vlen;
      
       if (token[0] == '<' && token[token.length()-1] == '>') {
         wildcards[token.substr(1, token.length()-2)] = string(vptr, vlen);
         index_delta -= 1;
       } else {
         index_delta -= token.length();
       }
     }
     return wildcards;
   }

  // map<string, string> pattern::match_wildcards(string comp) const {
  //   if (_pattern[0] == ':') {
  //     map<string, string> m;
  //     m[_pattern.substr(1, _pattern.length()-1)] = comp;
  //     return m;
  //   }
  //
  //   map<string, string> m;
  //   char *pattern = (char *)_pattern.c_str();
  //   char *str = (char *)comp.c_str();
  //
  //   for (list<unsigned>::const_iterator i = _wildcard_indeces.begin(); i != _wildcard_indeces.end(); ++i) {
  //     char *ptr = pattern+*i;
  //     string name(ptr+1,_advance_to_succ(ptr)-2);
  //
  //     char *vptr = str+*i;
  //     string value(vptr,_advance_to_str(vptr, ptr, _advance_to_char(ptr,'<')));
  //
  //     m[name] = value;
  //     str += value.length()-(name.length()+2);
  //   }
  //
  //   return m;
  // }
  
  list<string> pattern::match_splats(string cppstr) const {
    list<string> splats;
    char *pattern = (char *)_pattern.c_str();
    char *str = (char *)cppstr.c_str();
    int index_delta = 0;

    for (auto i = _index.begin(); i != _index.end(); i++) {
      unsigned idx = i->first;
      string token = i->second;
      
      char *succ = pattern+idx+token.length();
      char *vptr = str+idx+index_delta;
      char *tmpv = vptr;
      unsigned succlen = (i == _index.end()) ? 0 : next(i)->first-(idx+token.length());
      unsigned vlen = _advance_to_str(tmpv, succ, succlen);
      
      index_delta += vlen;
      
      if (token[0] == '*') {
        splats.push_back(string(vptr, vlen));
        index_delta -= 1;
      } else {
        index_delta -= token.length();
      }
    }
    return splats;
  }

  void pattern::create(string ptrn) {
    _pattern = ptrn;
    char *cptrn = (char *)_pattern.c_str();
    _index = _gen_indeces(cptrn,cptrn);
  }
  
  unsigned pattern::_advance_to_str(char *&ptr, char *str, unsigned n) const {
    char *start = ptr;
    while (*ptr != '\0' && (*str == '\0' || strncmp(ptr, str, n) != 0)) ptr++;
    return (unsigned)(ptr-start);
  }
  
  unsigned pattern::_advance_to_char(char *&ptr, char c) const {
    char *start = ptr;
    while (*ptr != '\0' && *ptr != c) ptr++;
    return (unsigned)(ptr-start);
  }

  unsigned pattern::_advance_to_wildcard(char *&ptr) const {
    char *start = ptr;
    while (*ptr != '\0' && *ptr != '*' && *ptr != '<') ptr++;
    return (unsigned)(ptr-start);
  }

  unsigned pattern::_advance_to_succ(char *&ptr) const {
    char *start = ptr;
    if (*ptr == '<') {
      while (*ptr != '>') ptr++; ptr++;
    } else if (*ptr == '*') {
      ptr++;
    }
    return (unsigned)(ptr-start);
  }

  // todo: implement using cached indeces
  bool pattern::_ptrncmp(char *pattern, char *comp) const {
    if (strlen(pattern) == 0) return true;
  
    char *tmp = pattern;
    if (_advance_to_succ(tmp) != 0) {
      pattern = tmp;
      char *succ = pattern;
      unsigned succlen = _advance_to_wildcard(pattern);      
      _advance_to_str(comp, succ, succlen);

      if (strncmp(succ, comp, succlen) != 0) return false;
      return _ptrncmp(pattern, comp+succlen);
    } else {
      if (pattern[0] != comp[0]) return false;
      return _ptrncmp(pattern+1, comp+1);
    }
  }
  
  map <unsigned, string> pattern::_gen_indeces(char *str, char *ptr) const {
    _advance_to_wildcard(ptr);
    if (*ptr == '\0') return map <unsigned, string>();
    unsigned idx = ptr-str;
    unsigned token_len = _advance_to_succ(ptr);
    
    map <unsigned, string> recursive = _gen_indeces(str, ptr);
    recursive[idx] = string(str+idx,token_len);
    return recursive;
  }
  //
  // list<unsigned> pattern::_get_splat_indeces_prime(char *str, char *ptr) const {
  //   _advance_to_char(ptr, '*');
  //   if (*ptr != '*') return list<unsigned>();
  //
  //   list<unsigned> recursive = _get_splat_indeces_prime(str, ptr+1);
  //   recursive.push_front(ptr-str);
  //   return recursive;
  // }
  //
  // list<unsigned> pattern::_get_wildcard_indeces_prime(char *str, char *ptr) const {
  //   _advance_to_char(ptr, '<');
  //   if (*ptr == '\0') return list<unsigned>();
  //
  //   list<unsigned> recursive = _get_wildcard_indeces_prime(str, ptr+1);
  //   recursive.push_front(ptr-str);
  //   return recursive;
  // }
}

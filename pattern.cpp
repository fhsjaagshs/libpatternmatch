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
    if (_wildcard_indeces.size() > 0) return true;
    if (_splat_indeces.size() > 0) return true;
    return false;
  }

  bool pattern::matches(string comp) const {
    if (_pattern == comp) return true;
    if (_pattern[0] == ':') return true;
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

  map<string, string> pattern::match_wildcards(string comp) const {
    if (_pattern[0] == ':') {
      map<string, string> m;
      m[_pattern.substr(1, _pattern.length()-1)] = comp;
      return m;
    }

    map<string, string> m;
    char *pattern = (char *)_pattern.c_str();
    char *str = (char *)comp.c_str();

    for (list<unsigned>::const_iterator i = _wildcard_indeces.begin(); i != _wildcard_indeces.end(); ++i) {
      char *ptr = pattern+*i;
      string name(ptr+1,_advance_to_succ(ptr)-2);

      char *vptr = str+*i;
      string value(vptr,_advance_to_str(vptr, ptr, _advance_to_char(ptr,'<')));

      m[name] = value;
      str += value.length()-(name.length()+2);
    }

    return m;
  }
  
  // asdf<sdfi>s* // 11
  // asdfs* // 5
  
  list<string> pattern::match_splats(string comp) const {
    list<string> splats;
    char *pattern = (char *)_pattern.c_str();
    char *str = (char *)comp.c_str();
    int index_delta = 0;

    cout << endl;

    for (list<unsigned>::const_iterator i = _splat_indeces.begin(); i != _splat_indeces.end(); ++i) {
      cout << "----------" << endl;

      char *succ = pattern+*i+1; // character after the splat
      char *vptr = str+*i+index_delta; // the analogous position in str

      cout << "succ: " << succ << endl;
      char *tmpsucc = succ;
      unsigned succlen = _advance_to_wildcard(tmpsucc);
      
      cout << "tmpsucc: " << tmpsucc << endl; 
      cout << "succlen: " << succlen << endl;
      
      cout << "vptr: " << vptr << endl;
      char *tmpv = vptr;
      unsigned vlen = _advance_to_str(tmpv, succ, succlen);
      cout << "vlen: " << vlen << endl;
      splats.push_back(string(vptr, vlen));
      
      if (*tmpsucc != '*' && *tmpsucc != '\0') {
        index_delta -= _advance_to_succ(tmpsucc); // account for non-splat wildcards
      }
      
      index_delta += (int)vlen-1;
    }

    return splats;
  }
  
  // list<string> pattern::_match_splats(char *str, char *pattern, list<string> &splats) const {
  //   cout << "----------" << endl;
  //   while (*pattern != '\0' && *pattern != '*') {
  //     if (*pattern == '<') while (*pattern != '>') pattern++;
  //     pattern++;
  //     str++;
  //   }
  //
  //   if (strlen(pattern) == 0) return splats;
  //
  //   cout << "pattern: " << pattern << endl;
  //   cout << "str: " << str << endl;
  //
  //   if (*pattern == '*') pattern++;
  //
  //
  //   char *tmp = pattern;
  //   char *tmpstr = str;
  //
  //   unsigned n = _advance_to_wildcard(tmp);
  //   unsigned vlen = _advance_to_str(tmpstr, pattern, n);
  //   cout << "succlen: " << n << endl;
  //   cout << "vlen: " << vlen << endl;
  //
  //   string value(tmpstr,vlen);
  //
  //   str += value.length();
  //
  //   cout << "pattern_after: " << pattern << endl;
  //   cout << "str_after: " << str << endl;
  //
  //   splats.push_back(value);
  //   return _match_splats(str,pattern,splats);
  // }

  void pattern::create(string ptrn) {
    _splat_indeces = _get_splat_indeces((char *)ptrn.c_str());
    _wildcard_indeces = _get_wildcard_indeces((char *)ptrn.c_str());
    _pattern = ptrn;
  }
  
  unsigned pattern::_advance_to_str(char *&ptr, char *str, unsigned n) const {
    char *start = ptr;
    while (*ptr != '\0' && (strncmp(ptr, str, n) != 0 || *str == '\0')) ptr++;
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

  list<unsigned> pattern::_get_splat_indeces_prime(char *str, char *ptr) const {
    _advance_to_char(ptr, '*');
    if (*ptr != '*') return list<unsigned>();
  
    list<unsigned> recursive = _get_splat_indeces_prime(str, ptr+1);
    recursive.push_front(ptr-str);
    return recursive;
  }

  list<unsigned> pattern::_get_wildcard_indeces_prime(char *str, char *ptr) const {
    _advance_to_char(ptr, '<');
    if (*ptr == '\0') return list<unsigned>();

    list<unsigned> recursive = _get_wildcard_indeces_prime(str, ptr+1);
    recursive.push_front(ptr-str);
    return recursive;
  }
}

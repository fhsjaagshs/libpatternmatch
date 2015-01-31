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

  list<string> pattern::match_splats(string comp) const {
    list<string> splats;
    char *pattern = (char *)_pattern.c_str();
    char *str = (char *)comp.c_str();
    
    for (list<unsigned>::const_iterator i = _splat_indeces.begin(); i != _splat_indeces.end(); ++i) {
      char *succ = pattern+*i; // character after the splat
      _advance_to_succ(succ);

      char *vptr = str+*i; // the analogous position in str
      string value(vptr, _advance_to_str(vptr, succ, _advance_to_wildcard(succ)));
      splats.push_back(value);
      str += value.length()-1;
    }
    
    return splats;
  }

  void pattern::create(string ptrn) {
    _splat_indeces = _get_splat_indeces((char *)ptrn.c_str());
    _wildcard_indeces = _get_wildcard_indeces((char *)ptrn.c_str());
    _pattern = ptrn;
  }
  
  unsigned pattern::_advance_to_str(char *&ptr, char *str, unsigned n) const {
    char *start = ptr;
    while (*ptr != '\0' && (strncmp(ptr, str, n) != 0  || *str == '\0')) ptr++;
    return (unsigned)(ptr-start);
  }
  
  unsigned pattern::_advance_to_char(char *&ptr, char c) const {
    char *start = ptr;
    while (*ptr != c && *ptr != '\0') ptr++;
    return (unsigned)(ptr-start);
  }

  unsigned pattern::_advance_to_wildcard(char *&ptr) const {
    char *start = ptr;
    while (*ptr != '*' && *ptr != '<' && *ptr != '\0') ptr++;
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

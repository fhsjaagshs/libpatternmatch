#include "pattern.h"

// TODO: Match last occurance of the 'succ' string when reading wildcards

// TODO: New wildcard: & matches 1 character

namespace LibPM {
  //
  // Public
  //
  
  bool pattern::is_dynamic() {
    if (_wildcard_indeces.size() > 0) return true;
    if (_splat_indeces.size() > 0) return true;
    return false;
  }

  bool pattern::matches(string comp) { 
    if (_pattern[0] == ':') return true;
    return _ptrncmp((char *)_pattern.c_str(), (char *)comp.c_str());
  }

  map<string, string> pattern::match_wildcards(string comp) {
    if (_pattern[0] == ':') {
      map<string, string> m;
      m[_pattern.substr(1, _pattern.length()-1)] = comp;
      return m;
    }
  
    map<string, string> m;
    char *pattern = (char *)_pattern.c_str();
    char *str = (char *)comp.c_str();
    int index_delta = 0;
  
    for (list<unsigned>::iterator i = _wildcard_indeces.begin(); i != _wildcard_indeces.end(); ++i) {
      unsigned idx = *i+index_delta;
      char *tmppattern = pattern+idx;
      string name = _read_until(tmppattern, '>');
      string succ = _read_until_wildcard(tmppattern);

      char *strtemp = str;

      _advance_to_str(strtemp,(char *)succ.c_str(),succ.length());

      unsigned valuelen = (unsigned)(strtemp-str);
  
      char *token = (char *)malloc(sizeof(char)*valuelen);
      strncpy(token,str,valuelen);
      string value(token);
      free(token);
      m[name] = value;
    }
  
    return m;
  }

  list<string> pattern::match_splats(string comp) {
    list<string> splats;
    char *pattern = (char *)_pattern.c_str();
    char *str = (char *)comp.c_str();
    int index_delta = 0;

    for (list<unsigned>::iterator i = _splat_indeces.begin(); i != _splat_indeces.end(); ++i) {
      unsigned idx = *i+index_delta;

      char *tmpptrn = pattern+*i; // character after the splat
      _advance_to_succ(tmpptrn);
      char *succ = tmpptrn;
      unsigned succlen = _advance_to_wildcard(tmpptrn);
      char *tmpstr = str+idx;
      
      unsigned splat_len = succlen > 0 ? _advance_to_str(tmpstr, succ, succlen) : strlen(tmpstr);

      char *buf = (char *)malloc(sizeof(char)*splat_len);
      strncpy(buf, (str+idx), splat_len);
      string value(buf, splat_len);
      splats.push_back(value);
      free(buf);
      index_delta += value.length()-1;
    }
    
    return splats;
  }

  //
  // Private
  //

  void pattern::create(string ptrn) {
    _splat_indeces = _get_splat_indeces((char *)ptrn.c_str());
    _wildcard_indeces = _get_wildcard_indeces((char *)ptrn.c_str());
    _pattern = ptrn;
  }
  
  unsigned pattern::_advance_to_str(char *&ptr, char *str, unsigned n) {
    char *start = ptr;
    while (/*ptr[n-1]*/ *ptr != '\0' && strncmp(str, ptr, n) != 0) ptr++;
    return (unsigned)(ptr-start);
  }

  unsigned pattern::_advance_to_wildcard(char *&ptr, char c) {
    char *start = ptr;
    if (c == '\0') {
      while (*ptr != '*' && *ptr != '<' && *ptr != '\0') ptr++;
    } else {
      while (*ptr != c && *ptr != '\0') ptr++;
    }
    return (unsigned)(ptr-start);
  }

  unsigned pattern::_advance_to_succ(char *&ptr) {
    char *start = ptr;
    if (*ptr == '<') {
      while (*ptr != '>') ptr++; ptr++;
    } else if (*ptr == '*') {
      ptr++;
    }
    return (unsigned)(ptr-start);
  }

  bool pattern::_ptrncmp(char *pattern, char *comp) {
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

  string pattern::_read_until(char *ptr, char c) {
    char *savedptr = ptr;
    while (*(ptr++) != c);
    unsigned len = ptr-savedptr;
    char *token = (char *)malloc(sizeof(char)*len);
    strncpy(token,savedptr,len);
    string read_str(token);
    free(token);
    return read_str;
  }

  string pattern::_read_until_wildcard(char *ptr) {
    char *savedptr = ptr;
    while (*ptr != '<' && *ptr != '*') ptr++;
    unsigned len = ptr-savedptr;
    char *token = (char *)malloc(sizeof(char)*len);
    strncpy(token,savedptr,len);
    string read_str(token);
    free(token);
    return read_str;
  }

  list<unsigned> pattern::_get_splat_indeces(char *str, char *ptr) {
    if (!ptr) ptr = str;

    _advance_to_wildcard(ptr, '*');
    
    unsigned idx = (unsigned)(ptr-str);
    
    if (*ptr != '*') return list<unsigned>();
  
    list<unsigned> recursive = _get_splat_indeces(str, ptr+1);
    recursive.push_front(idx);
    return recursive;
  }

  list<unsigned> pattern::_get_wildcard_indeces(char *str, char *ptr) {
    if (!ptr) ptr = str;
    if (str == ptr) return list<unsigned>();
  
    _advance_to_wildcard(ptr, '<');
    unsigned idx = (unsigned)(ptr-str);
    _advance_to_succ(ptr);
  
    list<unsigned> recursive = _get_wildcard_indeces(str, ptr);
    recursive.push_front(idx);
    return recursive;
  }
}

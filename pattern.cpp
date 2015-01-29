#include "pattern.h"

// TODO: Match last occurance of the 'succ' string when reading wildcards

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

      // advance to start of last occurance of the succ string in str
      while (strncmp(succ.c_str(), strtemp, succ.length()) != 0) strtemp++;
  
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

    cout << "count: " << _splat_indeces.size() << endl;

    for (list<unsigned>::iterator i = _splat_indeces.begin(); i != _splat_indeces.end(); ++i) {
      cout << "HERE" << endl;
      unsigned idx = *i+index_delta;
      char *tmpptrn = pattern+idx;
      _advance_to_succ(tmpptrn);
      char *succ = tmpptrn;
      unsigned succlen = _advance_to_wildcard(tmpptrn);
      char *tmpstr = str+idx;
      
      while (*(tmpstr+1) != '\0' && strncmp(succ, tmpstr, succlen) != 0) tmpstr++;
      
      unsigned splat_len = tmpstr-(str+idx);

      char *buf = (char *)malloc(sizeof(char)*splat_len);
      strncpy(buf, (str+idx), splat_len);
      string value(buf);
      splats.push_back(value);
      free(buf);
      index_delta += value.length();
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

  unsigned pattern::_advance_to_wildcard(char *&ptr, char c) {
    char *start = ptr;
    if (c == '\0') {
      while (*ptr != '*' && *ptr != '<' && *(ptr+1) != '\0') ptr++;
    } else {
      while (*ptr != c && *(ptr+1) != '\0') ptr++;
    }
    return (unsigned)(ptr-start);
  }

  unsigned pattern::_advance_to_succ(char *&ptr) {
    char *start = ptr;
    if (ptr[0] == '<') {
      while (*ptr != '>') ptr++; ptr++;
    } else if (ptr[0] == '*') {
      ptr++;
    }
    return (unsigned)(ptr-start);
  }

  bool pattern::_ptrncmp(char *pattern, char *comp) {
    if (strlen(pattern) == 0) return true;
  
    char *tmp = pattern;
    if (_advance_to_succ(tmp) != 0) { // if there are non-pattern characters after pattern
      pattern = tmp;
      char *succ = pattern;
      unsigned succlen = _advance_to_wildcard(pattern);

      while (*(comp+1) != '\0' && strncmp(succ, comp, succlen) != 0) comp++;

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

    char *tmp = ptr;
    if (str == ptr) {
      cout << "returning empty" << endl;
      return list<unsigned>();
    }
  
    _advance_to_wildcard(ptr, '*');
    cout << "\t(Splat IDX) Pointer: " << ptr << endl;
  
    list<unsigned> recursive = _get_splat_indeces(str, ptr+1);
    recursive.push_front((unsigned)(ptr-str));
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

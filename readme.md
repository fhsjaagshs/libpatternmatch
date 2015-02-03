# libpatternmatch

A fast pattern matching library designed for matching URL components in [sansomrb/sansom](http://github.com/sansomrb/sansom).

Usage:
-

    #include "pattern.h"
    
    using namespace std;
    
    int main (int argc, char **argv) {
        lpm::pattern p("*.<format>.<compression>");
    	string str = "users.json.tar.gz";
    	
    	// you can also use pattern.matches(str)
    	if (pattern == str) { 
    	  list<string> splats = p.extract_splats(str); // contains "users"
    	  map<string,string> wildcards = p.extract_mappings(str); // contains format:json and compression:tar.gz
    	}
    }
    
Patterns:
-

Patterns have the following special characters:

**\*** &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; (*splat*) matches any characters underneath it

**\<key\>** (*mapping*) matches any characters underneath it and maps the result to the value between the `<` and `>`.

For example:

    *.<format>.<compression>
    users.json.tar.gz # example
    
or

    <funcname>(*)
    matches(str) # example

    
Makefile Targets:
-

Just look in the makefile.


TODO:
-

- Make extensible: Subclass to add more wildcards
- Adopt makefile conventions
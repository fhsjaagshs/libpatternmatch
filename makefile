all: dylib shared static

dylib: pattern.o
	g++ -dynamiclib pattern.o -o libpatternmatch.dylib

shared: pattern.o
	g++ -shared pattern.o -o libpatternmatch.so

static:
	ar -cvq libpatternmatch.a pattern.o

pattern.o: pattern.cpp
	g++ -fPIC -std=c++11 -c pattern.cpp

tester.o: tester.cpp
	g++ -c tester.cpp

tester: pattern.o tester.o
	g++ pattern.o tester.o -o tester

test: tester
	make tester; ./tester '$(PATTERN)$(P)' '$(STR)$(S)'

clean:
	rm -rf *.o *.so *.dylib *.a tester

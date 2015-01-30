all: dylib shared

dylib: pattern.o
	g++ -dynamiclib -current_version 1.0 pattern.o -o libpatternmatch.dylib

shared: pattern.o
	g++ -shared pattern.o -o libpatternmatch.so

pattern.o: pattern.cpp
	g++ -fPIC -c pattern.cpp

tester.o: tester.cpp
	g++ -c tester.cpp

tester: pattern.o tester.o
	g++ pattern.o tester.o -o tester

test: tester
	./tester

clean:
	rm -rf *.o *.so *.dylib tester

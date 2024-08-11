all: libsamename.so libfuzzysame.so libregexp.so

libsamename.so:
	# gcc -shared -fPIC -lm -lsqlite3 samename.c -o $@
	g++ -O3 -fPIC -std=c++11 -shared samename.cpp -o $@ $(shell pkg-config --cflags sqlite3) -fPIC -W -Werror -Wno-unused-parameter
	cp libsamename.so libsamename.dylib

libfuzzysame.so:
	gcc -shared -fPIC -lm -lsqlite3 fuzzysame.c -o $@

libregexp.so:
	g++ -O3 -fPIC -std=c++11 -shared sqlite3-regexp.cpp -o $@ $(shell pkg-config --cflags sqlite3) -fPIC -W -Werror -Wno-unused-parameter -lboost_regex
	cp libregexp.so libregexp.dylib

clean:
	rm -f *.so
	rm -f *.dylib
	rm -f *.lib

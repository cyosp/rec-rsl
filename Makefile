prefix=/usr/local
CXXFLAGS=-std=c++11

all: rec-rsl
	
rec-rsl: rec-rsl.o
	$(CXX) $(CXXFLAGS) $+ -o $@ -lwiringPi

install: rec-rsl
	install -m 755 rec-rsl $(prefix)/bin

uninstall:
	$(RM) $(prefix)/bin/rec-rsl

clean:
	$(RM) *.o rec-rsl
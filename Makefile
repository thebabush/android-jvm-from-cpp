
CXXFLAGS := -std=gnu++11 -fPIE -pie -march=armv7-a -mthumb
LDFLAGS  := -Wl,--export-dynamic

hellohack: main.o sigchain.o hack.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ hack.o main.o sigchain.o

install: hellohack
	adb push hellohack /data/local/tmp

run:
	adb shell /data/local/tmp/hellohack

hack.cc:
	$(CXX) $(CXXFLAG) -c $@

main.cc:
	$(CXX) $(CXXFLAGS) -c $@

sigchain.cc:
	$(CXX)  $(CXXFLAGS) -c $@

.PHONY: clean

clean:
	rm -f *.o hellohack


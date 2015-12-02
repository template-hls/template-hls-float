CPPFLAGS += -W -Wall -std=c++11 -g
CPPFLAGS += -I ../../include
CPPFLAGS += -I /cygdrive/c/Usr/Xilinx2015.2/Vivado_HLS/2015.2/include

CPPFLAGS += -DNDEBUG=1 -O3

CPPFLAGS += -Wno-unknown-pragmas

LDLIBS += -lmpfr -lgmpxx -lgmp

%.ll : %.cpp
	clang -I ../../include -std=c++11 -O3 -DNDEBUG=1 -S -emit-llvm $< -o $@

%.s : %.cpp
	clang -I ../../include -std=c++11 -O3 -DNDEBUG=1 -S $< -o $@

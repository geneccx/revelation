SHELL = /bin/sh
SYSTEM = $(shell uname)
C++ = clang++
CC = clang
DFLAGS = 
LFLAGS = -L.
CFLAGS = -O3 -ID:/libs/boost/ -Wall -g -shared -Wl,--subsystem,windows -DDLL

OBJS = cmd_jvars.o cmd_strings.o Config.o DllMain.o Game.o JASS.o MHDetect.o mod_cheatpack_detect.o ModuleFactory.o Patcher.o Revelation.o
PROGS = ./Revelation.mixtape

all: $(OBJS) $(COBJS) $(PROGS)

clean:
	rm -f $(OBJS) $(COBJS) $(PROGS)

$(OBJS): %.o: %.cpp
	$(C++) -o $@ $(CFLAGS) -c $<

./Revelation.mixtape: $(OBJS) $(COBJS)

all: $(PROGS)

cmd_jvars.o: cmd_jvars.cpp
cmd_strings.o:
Config.o:
DllMain.o: 
Game.o: 
JASS.o: 
MHDetect.o: 
mod_cheatpack_detect.o: 
ModuleFactory.o: 
Patcher.o: 
Revelation.o: 
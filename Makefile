C++ = clang++
DFLAGS = 
LFLAGS = -L. -LD:/libs/boost/stage/lib -lboost_filesystem-mgw46-mt-1_51 -lboost_system-mgw46-mt-1_51
CFLAGS = -O3 -ID:/libs/boost/

OBJS = cmd_jvars.o cmd_strings.o Config.o Detour/CDetour.o Detour/CDetourDis.o DllMain.o Game.o JASS.o MHDetect.o mod_cheatpack_detect.o ModuleFactory.o Patcher.o Revelation.o
PROGS = Revelation.mixtape
ifeq ($(NPROCS),)
NPROCS = 8
all:
	$(MAKE) -j$(NPROCS) NPROCS=$(NPROCS)
else
all: $(OBJS) $(PROGS)
	copy /y Revelation.mixtape "E:/Games/Warcraft III/Revelation.mixtape"

clean:
	rm $(OBJS) $(PROGS)

$(OBJS): %.o: %.cpp
	$(C++) -o $@ $(CFLAGS) -c $<

Revelation.mixtape: $(OBJS)
	$(C++) $(CFLAGS) -shared -o Revelation.mixtape $(OBJS) $(COBJS) $(LFLAGS)

endif
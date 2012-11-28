C++ = clang
DFLAGS = -DCLANG
LFLAGS = -L.
CFLAGS = -Wno-invalid-token-paste -g0 -emit-llvm -O3 -ID:/libs/boost/ $(DFLAGS)
SRCS = $(wildcard *.cpp) $(wildcard Detour/*.cpp) 
OBJS = $(patsubst %.cpp,%.o,$(SRCS))
PROGS = Revelation.mixtape

all: $(OBJS) $(PROGS)
#	upx Revelation.mixtape
#	copy /y Revelation.mixtape "E:/Games/Warcraft III/Revelation.mixtape"
	
$(OBJS): %.o: %.cpp
	$(C++) $(CFLAGS) -o $@ -c $<
		
Revelation.mixtape: $(OBJS)
	llvm-link $(OBJS) -o revelation.bc
	llc -filetype=obj revelation.bc -o revelation.obj
#	$(C++) $(CFLAGS) -s -Wl,--version-script=exports.version -Wl,--subsystem,windows -shared -o $(PROGS) out/asm.s $(LFLAGS)
#-Wl,--gc-sections,--print-gc-sections 


#link revelation.obj /DLL /OUT:"D:\dev\Revelation\Release\Revelation.dll" msvcrt.lib libcmt.lib /NOLOGO /DYNAMICBASE /MACHINE:X86 /DLL /LIBPATH:"D:\libs\boost\stage\lib" /LIBPATH:"E:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\lib" /LIBPATH:"E:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\atlmfc\lib" /LIBPATH:"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\lib" /LIBPATH:"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\\lib"
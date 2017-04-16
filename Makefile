CC        = g++
LD        = g++

SONAME  = libPi433MHz.so
INC     = -Iinc/ -Idaemon/
SOSRC   = src/RemoteReceiver.cpp \
          src/RemoteTransmitter.cpp \
          src/RemoteControl.cpp \
          src/DeviceReceiver.cpp \
          src/DeviceTransmitter.cpp
DMNSRC  = daemon/Pi433MHzd.cpp \
		  daemon/config.cpp \
		  daemon/logger.cpp \
		  daemon/socket.cpp \
		  daemon/msg.cpp \
		  daemon/format.cpp
CLISRC  = comm/Pi433MHzif.cpp \
		  daemon/config.cpp \
		  daemon/logger.cpp \
		  daemon/socket.cpp \
		  daemon/format.cpp
CLITST  = comm/client.cpp 
SRVTST  = comm/server.cpp \
		  daemon/config.cpp \
		  daemon/logger.cpp \
		  daemon/socket.cpp \
		  daemon/msg.cpp \
		  daemon/format.cpp
TLELRO  = tools/elro.cpp
TLRAW   = tools/raw.cpp
TLRELRO = tools/relro.cpp
TLRRAW  = tools/rraw.cpp
NMELRO	= elro 
NMRAW   = raw
NMRELRO = relro 
NMRRAW  = rraw
NMDMN   = Pi433MHzd
NMCLI   = libPi433MHzif.so
NMSRTST = server
NMCLTST = client

# compilation options
CFLAGS  = -O3 -g -W -Wall $(INC)
# linking options
LDFLAGS = -shared -Wl,-soname,$(SONAME) -pthread -lpigpiod_if2 -lrt
LDFLAGS2 = -pthread -lpigpiod_if2 -lrt
LDFLAGS3 = -shared -Wl,-soname,$(NMCLI) -pthread -lrt

# how to compile individual object files
SOOBJS  = $(SOSRC:.cpp=.o)
OELRO   = $(TLELRO:.cpp=.o)
ORAW    = $(TLRAW:.cpp=.o)
ORELRO  = $(TLRELRO:.cpp=.o)
ORRAW   = $(TLRRAW:.cpp=.o)
ODAEMON = $(DMNSRC:.cpp=.o)
OCLIENT = $(CLISRC:.cpp=.o)
OSRVTST = $(SRVTST:.cpp=.o)
OCLITST = $(CLITST:.cpp=.o)
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean

# library compilation
$(SONAME): $(SOOBJS) $(SOSRC)
	$(CC) $(SOOBJS) $(LDFLAGS) -o $(SONAME)

$(NMCLI): $(OCLIENT) $(CLISRC)
	$(CC) $(OCLIENT) $(LDFLAGS3) -o $(NMCLI)

# cleaning rule
clean:
	rm -f $(SOOBJS) $(SONAME) $(OELRO) $(ORAW) $(ORELRO) $(ORRAW) $(ODAEMON) $(OCLIENT) $(OSRVTST) $(OCLITST) $(NMELRO) $(NMRAW) $(NMRELRO) $(NMRRAW) $(NMDMN) $(NMCLI) $(NMSRTST) $(NMCLTST) *~

lib: $(SONAME) $(NMCLI)

tools: $(OELRO) $(ORAW) $(ORELRO) $(ORRAW) $(TLELRO) $(TLRAW) $(TLRELRO) $(TLRRAW) $(SOOBJS) $(SOSRC) lib
	$(CC) $(OELRO) $(SOOBJS) $(LDFLAGS2) -o $(NMELRO)
	$(CC) $(ORAW) $(SOOBJS) $(LDFLAGS2) -o $(NMRAW) 
	$(CC) $(ORELRO) $(SOOBJS) $(LDFLAGS2) -o $(NMRELRO)
	$(CC) $(ORRAW) $(SOOBJS) $(LDFLAGS2) -o $(NMRRAW) 

daemon: $(ODAEMON) $(DMNSRC) $(SOOBJS) $(SOSRC) lib
	$(CC) $(ODAEMON) $(LDFLAGS2) -L. -lPi433MHz -o $(NMDMN)

comm: $(OSRVTST) $(OCLITST) $(SRVSRC) $(CLITST) lib
	$(CC) $(OSRVTST) $(LDFLAGS2) -o $(NMSRTST)
	$(CC) $(OCLITST) $(LDFLAGS2) -L. -lPi433MHzif -o $(NMCLTST)

install:
	./install.sh

uninstall:
	./install.sh -u

# additional rule
all: clean lib tools daemon comm

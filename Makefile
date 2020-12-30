PROG1	= gtts
OBJS1	= $(PROG1).cpp main.cpp
PROGS	= $(PROG1)

PKGS = gio-2.0 gio-unix-2.0 axhttp

CFLAGS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags $(PKGS))
LDLIBS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs $(PKGS))
CXXFLAGS += -O2 -mthumb -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a9 -fomit-frame-pointer $(CFLAGS)

all:	$(PROGS)

$(PROG1): $(OBJS1)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) $(LDLIBS) -o $@

clean:
	rm -f $(PROGS) *.o *.eap

AR := "$(shell (which arm-elf-ar arm-none-eabi-ar arm-linux-gnueabi-ar | head -1) 2>/dev/null)"
GCC = nspire-gcc
AS = nspire-as
GXX = nspire-g++
GCCFLAGS = -Wall -W -marm
LD = nspire-ld-bflt
LDFLAGS =
USERPROFILE ?= $(HOME)
DESTDIR = $(USERPROFILE)/.ndless

DEBUG = FALSE
ifeq ($(DEBUG),FALSE)
	GCCFLAGS += -Os
else
	GCCFLAGS += -O0 -g
	LDFLAGS += --debug
endif

GCCFLAGS += -Iinclude

CPPOBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
OBJS = $(patsubst %.c,%.o,$(wildcard *.c)) $(CPPOBJS)
ifneq ($(strip $(CPPOBJS)),)
	LDFLAGS += --cpp
endif

LIB = libnsocket.a

.PHONY: all lib clean install uninstall

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

%.o: %.cpp
	$(GXX) $(GCCFLAGS) -c $< -o $@
	
%.o: %.S
	$(AS) -c $< -o $@

all: lib

lib: $(LIB)

$(LIB): $(OBJS)
	mkdir -p lib
	$(AR) rcs "lib/$(LIB)" $^
	
install:
	cp nsocket.h "$(DESTDIR)/include/nsocket.h"
	cp lib/$(LIB) "$(DESTDIR)/lib"
	
uninstall:
	rm -rf "$(DESTDIR)/include/nsocket.h" "$(DESTDIR)/lib/$(LIB)"

clean:
	rm -f $(wildcard *.o) *.elf *.gdb lib/$(LIB)
# TODO non-64
CC := gcc
LD := gcc
CFLAGS := -W -Wall -I/z/Development/Nspire/ndless/Ndless-SDK/ndless_pc/include
LDFLAGS := -B/z/Development/Nspire/ndless/Ndless-SDK/ndless_pc/lib -lnavnet -lwsock32

OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

EXE := nsocket.exe

all: $(EXE)

$(EXE): $(OBJS)
	$(LD) -o $@ $< $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.exe *.o

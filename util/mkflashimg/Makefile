PROG	= mkflashimg
OBJS	= mkflashimg.o

INCLUDES=

BIN	= ../bin

CC	= gcc
STRIP	= strip
CHMOD	= chmod
RM	= rm -f
CP	= cp -fp

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) -o $(PROG) $(OBJS)
	$(STRIP) $(PROG)

install: $(PROG)
	$(CP) $(PROG) $(BIN)
	$(CHMOD) 775 $(BIN)/$(PROG)

clean:
	$(RM) $(BIN)/$(PROG) $(PROG) *.o *~

.c.o:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c -o $*.o $<


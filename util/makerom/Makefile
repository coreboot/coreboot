PROG	= makerom
OBJS	= makerom.o compress.o

COMMON          = ../common
COMMONLIB       = $(COMMON)/commlib.a

BIN	= ../bin

CC	= gcc
STRIP	= strip
CHMOD	= chmod
RM	= rm -f
CP	= cp -fp

CFLAGS	= -O2
INCLUDES = -I../include

all: $(PROG)

$(PROG): $(OBJS) $(COMMONLIB)
	$(CC) -o $(PROG) $(OBJS) $(COMMONLIB)
	$(STRIP) $(PROG)

install: $(PROG)
	$(CP) $(PROG) $(BIN)
	$(CHMOD) 775 $(BIN)/$(PROG)

clean:
	$(RM) $(BIN)/$(PROG) $(PROG) *.o *~

.c.o:
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c -o $*.o $<


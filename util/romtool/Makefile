COMMANDS=create.o bootblock.o delete.o add.o print.o resize.o
OBJ= $(COMMANDS) romtool.o util.o fs.o
H=romtool.h
DESTDIR=/usr/local/bin

all: romtool tools/rom-mkpayload tools/rom-mkstage

romtool: $(OBJ) 
	$(CC) -o $@ $(OBJ)

tools/rom-mkpayload tools/rom-mkstage:
	$(MAKE) -C tools/ $(patsubst tools/%, %, $@)

%.o: %.c
	$(CC) -g -Wall -Werror -c -o $@ $<

install: romtool tools/rom-mkpayload tools/rom-mkstage
	@ install -d $(DESTDIR)
	@ install -m 0755 romtool $(DESTDIR)/romtool
	@ install -m 0755 tools/rom-mkstage $(DESTDIR)/rom-mkstage
	@ install -m 0755 tools/rom-mkpayload $(DESTDIR)/rom-mkpayload

tags: 
	ctags *.[ch] */*.[ch]
clean:
	$(MAKE) -C tools/ clean
	rm -f *.o romtool

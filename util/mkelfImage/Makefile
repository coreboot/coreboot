# To run this: 
# Put the name of a directory in PREFIX where you want to run this stuff.
# make sure that this name is OUTSIDE THE LINUXBIOS TREE
# that way it survives across cvs updates. 
# Then do a 'make install'
# the result will be a directory tree  that you can run mkelfImage in
#PREFIX=/usr/local/
#PREFIX= /users/rminnich/src/bios/mkelf2
PERLPATH=/usr/bin/perl
VERSION="1.14"
DATE="28 June 2002"

SHAREDIR=$(PREFIX)/share/mkelfImage
BINDIR=$(PREFIX)/bin
MANDIR=$(PREFIX)/man/man1

DIRS=$(SHAREDIR) $(BINDIR) $(MANDIR)

MANS=mkelfImage.1

FILES=mkelfImage $(MANS) 

all: $(FILES)

clean:
	rm -f $(FILES)

distclean: clean
	find . -type f -name '*~' | xargs rm -f


install: $(DIRS) $(FILES)
	mkdir -p $(SHAREDIR) $(BINDIR) $(MANDIR)
	cp -fr elf32-i386/ $(SHAREDIR)
	find $(SHAREDIR) -type d | xargs chmod a+x
	find $(SHAREDIR) -type f | xargs chmod 444
	cp -f mkelfImage $(BINDIR)
	cp -f $(MANS) $(MANDIR)

$(DIRS):
	mkdir -p $@

%.1 : %.pl Makefile
	pod2man --date=$(DATE) --release=$(VERSION) $*.pl > $@

mkelfImage:  mkelfImage.pl Makefile
	echo 's|^$$params{MYDATA}=".";$$|$$params{MYDATA}="$(SHAREDIR)";|' > sedfile
	echo 's|^#!/usr/bin/perl|#!$(PERLPATH)|' >> sedfile
	echo 's|^my $$VERSION="";$$|my $$VERSION=$(VERSION);|' >> sedfile
	sed -f sedfile mkelfImage.pl > $@
	chmod a+x $@
	rm -f sedfile


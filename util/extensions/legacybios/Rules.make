# tag: Makefile rules

VPATH    := $(VPATH):.

.S.o:
	echo -n "  assembling $<... "
	$(CC) -c -nostdlib $(INCLUDES) $(CFLAGS) $< -o $(BUILDDIR)/$@ && \
		echo -e "    \t\tok" || \
		echo -e "    \t\tfailed"

.c.o:
	echo -n "  compiling $<... "
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $(BUILDDIR)/$@ && \
		echo -e "   \t\tok" || \
		echo -e "   \t\failed"
		


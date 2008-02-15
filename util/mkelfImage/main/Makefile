MKELF_OBJS=$(OBJDIR)/main/mkelfImage.o \
	$(OBJDIR)/linux-i386/mkelf-linux-i386.o \
	$(OBJDIR)/linux-ia64/mkelf-linux-ia64.o

$(OBJDIR)/sbin/mkelfImage: $(MKELF_OBJS) $(DEPS)
	$(MKDIR) -p $(@D)
	$(HOST_CC) $(HOST_CFLAGS) $(MKELF_OBJS) -o $@ $(LIBS)

$(OBJDIR)/main/mkelfImage.o: main/mkelfImage.c include/mkelfImage.h $(DEPS)
	$(MKDIR) -p $(@D)
	$(HOST_CC) $(HOST_CFLAGS) -c $< -o $@

$(OBJDIR)/man/man8/mkelfImage.8: main/mkelfImage.man
	$(MKDIR) -p $(@D)
	$(SED) \
		-e 's,^.TH MKELFIMAGE 8 "RELEASE_DATE" "VERSION"$$,.TH MKELFIMAGE 8 "$(RELEASE_DATE)" "$(VERSION)",' \
		$< > $@
	$(CP) $< $@

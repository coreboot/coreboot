LI386_DIR:=linux-i386
LI386_OBJ:=$(OBJDIR)/$(LI386_DIR)
LI386_DEP=Makefile Makefile.conf $(LI386_DIR)/Makefile

$(LI386_OBJ)/mkelf-linux-i386.o: $(LI386_DIR)/mkelf-linux-i386.c $(LI386_DIR)/convert.bin.c $(LI386_DEP)
	$(MKDIR) -p $(@D)
	$(HOST_CC) $(HOST_CFLAGS) -c -g $< -o $@


ifdef I386_CC

$(LI386_DIR)/convert.bin.c: $(LI386_OBJ)/convert.bin $(OBJDIR)/bin/bin-to-hex $(LI386_DEP)
	$(MKDIR) -p $(@D)
	$(OBJDIR)/bin/bin-to-hex < $(LI386_OBJ)/convert.bin > $@

$(LI386_OBJ)/convert.bin: $(LI386_OBJ)/convert $(LI386_DEP)
	$(MKDIR) -p $(@D)
	$(I386_OBJCOPY) -O binary $< $@

CONVERT_LI386_OBJS=$(LI386_OBJ)/head.o $(LI386_OBJ)/convert_params.o

$(LI386_OBJ)/convert: $(LI386_DIR)/convert.lds $(CONVERT_LI386_OBJS) $(LI386_DEP)
	$(MKDIR) -p $(@D)
	$(I386_LD) $(I386_LDFLAGS) -T $(LI386_DIR)/convert.lds -o $@ $(CONVERT_LI386_OBJS)

$(LI386_OBJ)/head.o: $(LI386_DIR)/head.S $(LI386_DIR)/convert.h
	$(MKDIR) -p $(@D)
	$(I386_CPP) $(I386_CPPFLAGS) -DASSEMBLY $< | $(I386_AS) $(I386_ASFLAGS) -o $@

$(LI386_OBJ)/convert_params.o: $(LI386_DIR)/convert_params.c $(LI386_DIR)/convert.h $(LI386_DEP)
	$(MKDIR) -p $(@D)
	$(I386_CC) $(I386_CFLAGS) -c -o $@ $<

endif


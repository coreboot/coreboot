LIA64_DIR=linux-ia64
LIA64_OBJ=$(OBJDIR)/$(LIA64_DIR)
LIA64_DEP=Makefile Makefile.conf $(LIA64_DIR)/Makefile

$(LIA64_OBJ)/mkelf-linux-ia64.o: $(LIA64_DIR)/mkelf-linux-ia64.c $(LIA64_DIR)/convert.bin.c $(LIA64_DEP)
	$(MKDIR) -p $(@D)
	$(HOST_CC) $(HOST_CFLAGS) -c -g $< -o $@

ifdef IA64_CC

$(LIA64_DIR)/convert.bin.c: $(LIA64_OBJ)/convert.bin $(OBJDIR)/bin/bin-to-hex $(LIA64_DEP)
	$(MKDIR) -p $(@D)
	$(OBJDIR)/bin/bin-to-hex < $(LIA64_OBJ)/convert.bin > $@

$(LIA64_OBJ)/convert.bin: $(LIA64_OBJ)/convert $(LIA64_DEP)
	$(MKDIR) -p $(@D)
	$(IA64_OBJCOPY) -O binary $< $@

CONVERT_LIA64_OBJS=$(LIA64_OBJ)/head.o $(LIA64_OBJ)/convert_params.o
$(LIA64_OBJ)/convert: $(LIA64_DIR)/convert.lds $(CONVERT_LIA64_OBJS) $(LIA64_DEP)
	$(MKDIR) -p $(@D)
	$(IA64_LD) $(IA64_LDFLAGS) -T $(LIA64_DIR)/convert.lds -o $@ $(CONVERT_LIA64_OBJS)

$(LIA64_OBJ)/head.o: $(LIA64_DIR)/head.S $(LIA64_DIR)/convert.h
	$(MKDIR) -p $(@D)
	$(IA64_CPP) $(IA64_CFLAGS) -D ASSEMBLY $< |  $(IA64_AS) $(IA64_ASFLAGS) -o $@

$(LIA64_OBJ)/convert_params.o: $(LIA64_DIR)/convert_params.c $(LIA64_DIR)/convert.h $(LIA64_DEP)
	$(IA64_CC) $(IA64_CFLAGS) -c -o $@ $<


endif


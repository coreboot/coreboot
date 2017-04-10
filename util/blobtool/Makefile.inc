blobtool_obj := blobtool.lex.o blobtool.tab.o

BLOBTOOL_FLAGS += -I$(top)/util/blobtool -I$(objutil)/blobtool

$(objutil)/blobtool:
	mkdir -p $@

$(objutil)/blobtool/.generated: $(objutil)/blobtool
	touch $@

$(objutil)/blobtool/%.o: util/blobtool/%.c | $(objutil)/blobtool/.generated
	printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(BLOBTOOL_FLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/blobtool/%.o: $(objutil)/blobtool/%.c
	printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(BLOBTOOL_FLAGS) $(HOSTCFLAGS) -c -o $@ $<

ifeq ($(CONFIG_UTIL_GENPARSER),y)
$(top)/util/blobtool/blobtool.lex.c_shipped: $(top)/util/blobtool/blobtool.l
	printf "    FLEX       $(subst $(top)/,,$(@))\n"
	flex -L -o $@ $<

# the .c rule also creates .h
$(top)/util/blobtool/blobtool.tab.h_shipped: $(top)/util/blobtool/blobtool.tab.c_shipped
$(top)/util/blobtool/blobtool.tab.c_shipped: $(top)/util/blobtool/blobtool.y
	printf "    BISON      $(subst $(top)/,,$(@))\n"
	bison -l --defines=$(top)/util/blobtool/blobtool.tab.h_shipped -o $@ $<
endif

$(objutil)/blobtool/blobtool.lex.o: $(objutil)/blobtool/blobtool.tab.h

$(objutil)/blobtool/%: $(top)/util/blobtool/%_shipped
	mkdir -p $(dir $@)
	cp $< $@

$(objutil)/blobtool/blobtool: $(addprefix $(objutil)/blobtool/,$(blobtool_obj))
	printf "    HOSTCC     $(subst $(obj)/,,$(@)) (link)\n"
	$(HOSTCC) $(BLOBTOOL_FLAGS) -o $@ $(addprefix $(objutil)/blobtool/,$(blobtool_obj))

$(addprefix $(objutil)/blobtool/,$(blobtool_obj)) : $(objutil)/blobtool/blobtool.tab.h $(objutil)/blobtool/blobtool.tab.c $(objutil)/blobtool/blobtool.lex.c

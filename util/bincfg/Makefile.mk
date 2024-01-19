## SPDX-License-Identifier: GPL-2.0-only
bincfg_obj := bincfg.lex.o bincfg.tab.o

BINCFG_FLAGS += -I$(top)/util/bincfg -I$(objutil)/bincfg

$(objutil)/bincfg:
	mkdir -p $@

$(objutil)/bincfg/.generated: $(objutil)/bincfg
	touch $@

$(objutil)/bincfg/%.o: util/bincfg/%.c | $(objutil)/bincfg/.generated
	printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(BINCFG_FLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/bincfg/%.o: $(objutil)/bincfg/%.c
	printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(BINCFG_FLAGS) $(HOSTCFLAGS) -c -o $@ $<

ifeq ($(CONFIG_UTIL_GENPARSER),y)
$(top)/util/bincfg/bincfg.lex.c_shipped: $(top)/util/bincfg/bincfg.l
	printf "    FLEX       $(subst $(top)/,,$(@))\n"
	flex -L -o $@ $<

# the .c rule also creates .h
$(top)/util/bincfg/bincfg.tab.h_shipped: $(top)/util/bincfg/bincfg.tab.c_shipped
$(top)/util/bincfg/bincfg.tab.c_shipped: $(top)/util/bincfg/bincfg.y
	printf "    BISON      $(subst $(top)/,,$(@))\n"
	bison -l --defines=$(top)/util/bincfg/bincfg.tab.h_shipped -o $@ $<
endif

$(objutil)/bincfg/bincfg.lex.o: $(objutil)/bincfg/bincfg.tab.h

$(objutil)/bincfg/%: $(top)/util/bincfg/%_shipped
	mkdir -p $(dir $@)
	cp $< $@

$(objutil)/bincfg/bincfg: $(addprefix $(objutil)/bincfg/,$(bincfg_obj))
	printf "    HOSTCC     $(subst $(obj)/,,$(@)) (link)\n"
	$(HOSTCC) $(BINCFG_FLAGS) -o $@ $(addprefix $(objutil)/bincfg/,$(bincfg_obj))

$(addprefix $(objutil)/bincfg/,$(bincfg_obj)) : $(objutil)/bincfg/bincfg.tab.h $(objutil)/bincfg/bincfg.tab.c $(objutil)/bincfg/bincfg.lex.c

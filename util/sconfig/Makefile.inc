sconfigobj :=
sconfigobj += lex.yy.o
sconfigobj += sconfig.tab.o
sconfigobj += main.o

SCONFIGFLAGS += -I$(top)/util/sconfig -I$(objutil)/sconfig

$(objutil)/sconfig:
	mkdir -p $@

$(objutil)/sconfig/%.o: util/sconfig/%.c
	printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(SCONFIGFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/sconfig/%.o: $(objutil)/sconfig/%.c
	printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(SCONFIGFLAGS) $(HOSTCFLAGS) -c -o $@ $<

ifdef CONFIG_SCONFIG_GENPARSER
$(top)/util/sconfig/lex.yy.c_shipped: $(top)/util/sconfig/sconfig.l
	printf "    FLEX       $(subst $(top)/,,$(@))\n"
	flex -L -o $@ $<

# the .c rule also creates .h
$(top)/util/sconfig/sconfig.tab.h_shipped: $(top)/util/sconfig/sconfig.tab.c_shipped
$(top)/util/sconfig/sconfig.tab.c_shipped: $(top)/util/sconfig/sconfig.y
	printf "    BISON      $(subst $(top)/,,$(@))\n"
	bison -l --defines=$(top)/util/sconfig/sconfig.tab.h_shipped -o $@ $<

endif

$(objutil)/sconfig/lex.yy.o: $(objutil)/sconfig/sconfig.tab.h

$(objutil)/sconfig/%: $(top)/util/sconfig/%_shipped
	mkdir -p $(dir $@)
	cp $< $@

$(objutil)/sconfig/sconfig: $(objutil)/sconfig $(addprefix $(objutil)/sconfig/,$(sconfigobj))
	printf "    HOSTCC     $(subst $(obj)/,,$(@)) (link)\n"
	$(HOSTCC) $(SCONFIGFLAGS) -o $@ $(addprefix $(objutil)/sconfig/,$(sconfigobj))

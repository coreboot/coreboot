sconfigobj :=
sconfigobj += lex.yy.o
sconfigobj += sconfig.tab.o

$(obj)/util/sconfig:
	mkdir -p $@

$(obj)/util/sconfig/%.o: $(obj)/util/sconfig/%.c
	printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(SCONFIGFLAGS) $(HOSTCFLAGS) -c -o $@ $<

ifdef SCONFIG_GENPARSER
$(top)/util/sconfig/lex.yy.c_shipped: $(top)/util/sconfig/sconfig.l
	flex -o $@ $<

# the .c rule also creates .h
$(top)/util/sconfig/sconfig.tab.h_shipped: $(top)/util/sconfig/sconfig.tab.c_shipped
$(top)/util/sconfig/sconfig.tab.c_shipped: $(top)/util/sconfig/sconfig.y
	bison --defines=$(top)/util/sconfig/sconfig.tab.h_shipped -o $@ $<

endif

$(obj)/util/sconfig/lex.yy.o: $(obj)/util/sconfig/sconfig.tab.h

$(obj)/util/sconfig/%: $(top)/util/sconfig/%_shipped
	cp $< $@

$(obj)/util/sconfig/sconfig: $(obj)/util/sconfig $(addprefix $(obj)/util/sconfig/,$(sconfigobj))
	printf "    HOSTCXX    $(subst $(obj)/,,$(@)) (link)\n"
	$(HOSTCXX) $(SCONFIGFLAGS) -o $@ $(addprefix $(obj)/util/sconfig/,$(sconfigobj))

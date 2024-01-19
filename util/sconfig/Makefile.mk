## SPDX-License-Identifier: GPL-2.0-only
sconfigobj :=
sconfigobj += lex.yy.o
sconfigobj += sconfig.tab.o
sconfigobj += main.o

SCONFIGFLAGS += -I$(top)/util/sconfig -I$(objutil)/sconfig
SCONFIGFLAGS += -I$(top)/src/commonlib/include -I$(top)/src/commonlib/bsd/include

$(objutil)/sconfig:
	mkdir -p $@

$(objutil)/sconfig/.generated: $(objutil)/sconfig
	touch $@

$(objutil)/sconfig/%.o: util/sconfig/%.c | $(objutil)/sconfig/.generated
	printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(SCONFIGFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/sconfig/%.o: $(objutil)/sconfig/%.c
	printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(SCONFIGFLAGS) $(HOSTCFLAGS) -c -o $@ $<

ifeq ($(CONFIG_UTIL_GENPARSER),y)
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

$(objutil)/sconfig/sconfig: $(addprefix $(objutil)/sconfig/,$(sconfigobj))
	printf "    HOSTCC     $(subst $(obj)/,,$(@)) (link)\n"
	$(HOSTCC) $(SCONFIGFLAGS) -o $@ $(addprefix $(objutil)/sconfig/,$(sconfigobj))

$(addprefix $(objutil)/sconfig/,$(sconfigobj)) : $(objutil)/sconfig/sconfig.tab.h $(objutil)/sconfig/sconfig.tab.c $(objutil)/sconfig/lex.yy.c

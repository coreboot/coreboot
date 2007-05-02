TARGETS = dtc ftdump
CFLAGS = -Wall -g

BISON = bison

DTC_OBJS = dtc.o livetree.o flattree.o data.o treesource.o fstree.o \
		dtc-parser.tab.o lex.yy.o

OBJS = $(DTC_OBJS) libdt.o ftdump.o

DEPFILES = $(DTC_OBJS:.o=.d)

#all: $(TARGETS)

$(obj)/util/dtc/dtc: $(patsubst %,$(obj)/util/dtc/%,$(DTC_OBJS))
	$(Q)mkdir -p $(obj)/util/dtc/
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) -o $@ $^

ftdump:	ftdump.o
	$(Q)$(HOSTCC) -o $@ $^

$(obj)/util/dtc/dtc-parser.tab.c $(obj)/util/dtc/dtc-parser.tab.h $(obj)/util/dtc/dtc-parser.output: $(src)/util/dtc/dtc-parser.y
	$(Q)mkdir -p $(obj)/util/dtc
	$(Q)printf "  BISON   $(subst $(shell pwd)/,,$(@))\n"
	$(Q)cd $(obj)/util/dtc; $(BISON) -d $< &> $(obj)/util/dtc/dtc.bison.out

$(obj)/util/dtc/lex.yy.c: $(src)/util/dtc/dtc-lexer.l
	$(Q)cd $(obj)/util/dtc; $(LEX) $<

$(obj)/util/dtc/%.o: $(src)/util/dtc/%.c
	$(Q)mkdir -p $(obj)/util/dtc/
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) -I$(src)/util/dtc $(HOSTCFLAGS) -o $@ -c $< 

$(obj)/util/dtc/dtc-parser.tab.o: $(obj)/util/dtc/dtc-parser.tab.c
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) -I$(src)/util/dtc $(HOSTCFLAGS) -o $@ -c $< 

$(obj)/util/dtc/lex.yy.o: $(obj)/util/dtc/lex.yy.c $(obj)/util/dtc/dtc-parser.tab.h
	$(Q)$(HOSTCC) -I$(src)/util/dtc $(HOSTCFLAGS) -o $@ -c $< 

ifdef DTC_DEBUG

lex.yy.o: lex.yy.c dtc-parser.tab.h

check: all
	$(Q)cd tests && $(MAKE) check

clean:
	$(Q)rm -f *~ *.o a.out core $(TARGETS)
	$(Q)rm -f *.tab.[ch] lex.yy.c
	$(Q)rm -f *.i *.output vgcore.*
	$(Q)rm -f *.d
	$(Q)cd tests && $(MAKE) clean
endif

%.d: %.c
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(HOSTCC) -MM -MG -MT "$*.o $@" $< > $@

-include $(DEPFILES)

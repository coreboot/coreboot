TARGETS = dtc ftdump
CFLAGS = -Wall -g

BISON = bison

DTC_OBJS = dtc.o livetree.o flattree.o data.o treesource.o fstree.o \
		dtc-parser.tab.o lex.yy.o

OBJS = $(DTC_OBJS) libdt.o ftdump.o

DEPFILES = $(DTC_OBJS:.o=.d)

#all: $(TARGETS)

$(obj)/util/dtc/dtc: $(patsubst %,$(obj)/%,$(DTC_OBJS))
	$(Q)mkdir -p $(obj)/util/dtc/
	$(Q)$(LINK.c) -o $@ $^

ftdump:	ftdump.o
	$(Q)$(LINK.c) -o $@ $^

$(obj)/dtc-parser.tab.c $(obj)/dtc-parser.tab.h $(obj)/dtc-parser.output: $(src)/util/dtc/dtc-parser.y
	$(Q)cd $(obj); $(BISON) -d $< &> $(obj)/dtc.bison.out

$(obj)/lex.yy.c: $(src)/util/dtc/dtc-lexer.l
	$(Q)cd $(obj); $(LEX) $<

$(obj)/dtc.o: $(src)/util/dtc/dtc.c
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -o $@ -c $< 
$(obj)/livetree.o: $(src)/util/dtc/livetree.c
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -o $@ -c $< 
$(obj)/flattree.o: $(src)/util/dtc/flattree.c
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -o $@ -c $< 
$(obj)/data.o: $(src)/util/dtc/data.c
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -o $@ -c $< 
$(obj)/treesource.o: $(src)/util/dtc/treesource.c
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -o $@ -c $< 
$(obj)/fstree.o: $(src)/util/dtc/fstree.c
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -o $@ -c $< 
$(obj)/dtc-parser.tab.o: $(obj)/dtc-parser.tab.c
	$(Q)$(HOSTCC) -I$(src)/util/dtc $(HOSTCFLAGS) -o $@ -c $< 
$(obj)/lex.yy.o: $(obj)/lex.yy.c $(obj)/dtc-parser.tab.h
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
	$(CC) -MM -MG -MT "$*.o $@" $< > $@

-include $(DEPFILES)

TARGETS = dtc ftdump
CFLAGS = -Wall -g

BISON = bison

DTC_OBJS = dtc.o livetree.o flattree.o data.o treesource.o fstree.o \
		dtc-parser.tab.o lex.yy.o

OBJS = $(DTC_OBJS) libdt.o ftdump.o

DEPFILES = $(DTC_OBJS:.o=.d)

#all: $(TARGETS)

$(obj)/util/dtc/dtc: $(patsubst %,$(obj)/%,$(DTC_OBJS))
	mkdir -p $(obj)/util/dtc/
	$(LINK.c) -o $@ $^

ftdump:	ftdump.o
	$(LINK.c) -o $@ $^

$(obj)/dtc-parser.tab.c $(obj)/dtc-parser.tab.h $(obj)/dtc-parser.output: $(src)/util/dtc/dtc-parser.y
	cd $(obj); $(BISON) -d $< 

$(obj)/lex.yy.c: $(src)/util/dtc/dtc-lexer.l
	cd $(obj); $(LEX) $<

#lex.yy.o: lex.yy.c dtc-parser.tab.h

check: all
	cd tests && $(MAKE) check

$(obj)/dtc.o: $(src)/util/dtc/dtc.c
	$(HOSTCC) $(HOSTCFLAGS) -o $@ -c $< 
$(obj)/livetree.o: $(src)/util/dtc/livetree.c
	$(HOSTCC) $(HOSTCFLAGS) -o $@ -c $< 
$(obj)/flattree.o: $(src)/util/dtc/flattree.c
	$(HOSTCC) $(HOSTCFLAGS) -o $@ -c $< 
$(obj)/data.o: $(src)/util/dtc/data.c
	$(HOSTCC) $(HOSTCFLAGS) -o $@ -c $< 
$(obj)/treesource.o: $(src)/util/dtc/treesource.c
	$(HOSTCC) $(HOSTCFLAGS) -o $@ -c $< 
$(obj)/fstree.o: $(src)/util/dtc/fstree.c
	$(HOSTCC) $(HOSTCFLAGS) -o $@ -c $< 
$(obj)/dtc-parser.tab.o: $(obj)/dtc-parser.tab.c
	$(HOSTCC) -I$(src)/util/dtc $(HOSTCFLAGS) -o $@ -c $< 
$(obj)/lex.yy.o: $(obj)/lex.yy.c $(obj)/dtc-parser.tab.h
	$(HOSTCC) -I$(src)/util/dtc $(HOSTCFLAGS) -o $@ -c $< 

#clean:
#	rm -f *~ *.o a.out core $(TARGETS)
#	rm -f *.tab.[ch] lex.yy.c
#	rm -f *.i *.output vgcore.*
#	rm -f *.d
#	cd tests && $(MAKE) clean


%.d: %.c
	$(CC) -MM -MG -MT "$*.o $@" $< > $@

-include $(DEPFILES)

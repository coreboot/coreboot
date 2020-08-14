CC = gcc
YACC = bison
LEX = flex
TARGET=bincfg

$(TARGET): $(TARGET).lex.o $(TARGET).tab.o
	$(CC) $^ -Wall -Wno-unused-function -g -lfl -o $@

$(TARGET).lex.c: $(TARGET).l $(TARGET).tab.h
	$(LEX) -o $(patsubst $(TARGET).l,$(TARGET).lex.c,$<) $<

$(TARGET).tab.c $(TARGET).tab.h: $(TARGET).y
	$(YACC) -d $<

# Use this target to generate GbE for X200
gen-gbe-ich9m:
	./bincfg gbe-ich9m.spec gbe-ich9m.set gbe1.bin
	# duplicate binary as per spec
	cat gbe1.bin gbe1.bin > flashregion_3_gbe.bin
	rm -f gbe1.bin

# Use this target to generate GbE for X220/x230
gen-gbe-82579LM:
	./bincfg gbe-82579LM.spec gbe-82579LM.set gbe1.bin
	# duplicate binary as per spec
	cat gbe1.bin gbe1.bin > flashregion_3_gbe.bin
	rm -f gbe1.bin

# Use this target to generate IFD for X200
gen-ifd-x200:
	./bincfg ifd-x200.spec ifd-x200.set flashregion_0_fd.bin

.PHONY: clean gen-gbe-ich9m gen-ifd-x200

clean:
	rm -f *.lex.c *.tab.c *.tab.h *.o bincfg flashregion_0_fd.bin flashregion_3_gbe.bin

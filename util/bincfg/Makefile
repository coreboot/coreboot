## SPDX-License-Identifier: GPL-2.0-only
CC = gcc
YACC = bison
LEX = flex
TARGET=bincfg
WERROR=-Werror
CFLAGS=-O2 -Wall -Wextra -Wshadow ${WERROR}
CFLAGS+=-Wno-unused-function
LDFLAGS= -lfl

all: $(TARGET)

$(TARGET): $(TARGET).lex.o $(TARGET).tab.o
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

$(TARGET).lex.c: $(TARGET).l $(TARGET).tab.h
	$(LEX) -o $(patsubst $(TARGET).l,$(TARGET).lex.c,$<) $<

$(TARGET).tab.c $(TARGET).tab.h: $(TARGET).y
	$(YACC) -d $<

# Use this target to generate GbE for X200
gen-gbe-ich9m: $(TARGET)
	$(abspath $(TARGET)) gbe-ich9m.spec gbe-ich9m.set gbe1.bin
	# duplicate binary as per spec
	cat gbe1.bin gbe1.bin > flashregion_3_gbe.bin
	rm -f gbe1.bin

# Use this target to generate GbE for X220/x230
gen-gbe-82579LM: $(TARGET)
	$(abspath $(TARGET)) gbe-82579LM.spec gbe-82579LM.set gbe1.bin
	# duplicate binary as per spec
	cat gbe1.bin gbe1.bin > flashregion_3_gbe.bin
	rm -f gbe1.bin

# Use this target to generate IFD for X200
gen-ifd-x200: $(TARGET)
	$(abspath $(TARGET)) ifd-x200.spec ifd-x200.set flashregion_0_fd.bin

clean:
	rm -f *.lex.c *.tab.c *.tab.h *.o bincfg flashregion_0_fd.bin flashregion_3_gbe.bin

distclean: clean

help:
	@echo "${TARGET}: Compiler/Decompiler for data blobs with specs"
	@echo "Targets: all, clean, distclean, help"
	@echo "  gen-gbe-ich9m   - generate GbE for X200"
	@echo "  gen-gbe-82579LM - generate GbE for X220/x230"
	@echo "  gen-ifd-x200    - generate IFD for X200"
	@echo "To disable warnings as errors, run make as:"
	@echo "  make all WERROR=\"\""

.PHONY: all clean distclean help
.PHONY: gen-gbe-ich9m gen-ifd-x200 gen-gbe-82579LM

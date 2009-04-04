tobj ?= $(shell pwd)
tsrc ?= $(shell pwd)

TARGETS += $(tobj)/rom-mkstage $(tobj)/rom-mkpayload

tools: $(tobj)/rom-mkstage $(tobj)/rom-mkpayload

include $(tobj)/lzma/Makefile

COMMON = common.o compress.o $(LZMA_OBJ)

$(tobj)/rom-mkstage: $(tobj)/rom-mkstage.o $(patsubst %,$(tobj)/%,$(COMMON))
	$(CXX) $(CFLAGS) -o $@ $^

$(tobj)/rom-mkpayload: $(tobj)/rom-mkpayload.o $(patsubst %,$(tobj)/%,$(COMMON))
	$(CXX) $(CFLAGS) -o $@ $^

$(tobj)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

tools-clean:
	rm -f $(tobj)/rom-mkpayload.o $(tobj)/rom-mkstage.o $(patsubst %,$(tobj)/%,$(COMMON))
	rm -f $(tobj)/rom-mkpayload $(tobj)/rom-mkstage


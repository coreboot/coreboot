tobj ?= $(shell pwd)
tsrc ?= $(shell pwd)

TARGETS += $(tobj)/cbfs-mkstage $(tobj)/cbfs-mkpayload

tools: $(tobj)/cbfs-mkstage $(tobj)/cbfs-mkpayload

include $(tsrc)/lzma/Makefile

COMMON = common.o compress.o $(LZMA_OBJ)

$(tobj)/cbfs-mkstage: $(tobj)/cbfs-mkstage.o $(patsubst %,$(tobj)/%,$(COMMON))
	$(CXX) $(CFLAGS) -o $@ $^

$(tobj)/cbfs-mkpayload: $(tobj)/cbfs-mkpayload.o $(patsubst %,$(tobj)/%,$(COMMON))
	$(CXX) $(CFLAGS) -o $@ $^

$(tobj)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

tools-clean:
	rm -f $(tobj)/cbfs-mkpayload.o $(tobj)/cbfs-mkstage.o $(patsubst %,$(tobj)/%,$(COMMON))
	rm -f $(tobj)/cbfs-mkpayload $(tobj)/cbfs-mkstage


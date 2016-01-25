LPGCC = ../libpayload/install/libpayload/bin/lpgcc

all: nvramcui.elf

%.elf:
	$(LPGCC) -o $*.elf $*.c

.PHONY:
clean:
	rm -f nvramcui.elf

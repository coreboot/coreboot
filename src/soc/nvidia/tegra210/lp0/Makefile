## SPDX-License-Identifier: GPL-2.0-only

xcompile ?= ../../../../../build/xcompile
-include $(xcompile)

GCC_PREFIX?=$(CROSS_COMPILE_arm)

CC = $(GCC_PREFIX)gcc
NM = $(GCC_PREFIX)nm
OBJCOPY = $(GCC_PREFIX)objcopy

OPENSSL = openssl
DD = dd
CP = cp
MV = mv
RM = rm

SIGKEY = 00000000000000000000000000000000

.PHONY: all
all: tegra_lp0_resume.fw

tegra_lp0_resume.elf: tegra_lp0_resume.ld tegra_lp0_resume.c
	$(CC) -marm -march=armv4t -mno-unaligned-access -nostdlib -static \
		-Os -fpie -Wl,--build-id=none -ggdb3 -T tegra_lp0_resume.ld \
		-include ../../../../include/stdint.h \
		-include ../../../../commonlib/bsd/include/commonlib/bsd/compiler.h \
		-o $@ $(filter %.c,$+)

tegra_lp0_resume.fw: tegra_lp0_resume.elf
	@# Get rid of any files we're about to create.
	$(RM) -f $@.nosig $@.sig $@.tosig
	@# Convert the ELF image into a binary image.
	$(OBJCOPY) -O binary $< $@.nosig
	@# Extract the part of the binary which needs to be signed.
	$(DD) bs=1 skip=544 if=$@.nosig of=$@.tosig
	@# Calculate a signature for that part.
	$(OPENSSL) dgst -mac cmac -macopt cipher:aes-128-cbc \
		-macopt hexkey:$(SIGKEY) -md5 -binary \
		$@.tosig > $@.sig
	@# Inject the signature into the binary image's header.
	$(DD) conv=notrunc bs=1 seek=272 count=16 if=$@.sig of=$@.nosig
	@# Copy the signed binary to the target file name.
	$(MV) $@.nosig $@

distclean clean:
	$(RM) -f tegra_lp0_resume.fw tegra_lp0_resume.fw.sig
	$(RM) -f tegra_lp0_resume.fw.tosig tegra_lp0_resume.elf

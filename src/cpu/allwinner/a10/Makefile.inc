bootblock-y	+= bootblock.c
bootblock-y	+= bootblock_media.c
bootblock-y	+= clock.c
bootblock-y	+= gpio.c
bootblock-y	+= pinmux.c
bootblock-y	+= raminit.c
bootblock-y	+= timer.c

romstage-y	+= bootblock_media.c
romstage-y	+= cbmem.c
romstage-y	+= clock.c
romstage-y	+= pinmux.c
romstage-y	+= timer.c
romstage-y	+= twi.c

ramstage-y	+= bootblock_media.c
ramstage-y	+= cbmem.c
ramstage-y	+= clock.c
ramstage-y	+= cpu.c
ramstage-y	+= monotonic_timer.c
ramstage-y	+= timer.c
ramstage-y	+= twi.c

ifeq ($(CONFIG_DRIVERS_UART),y)
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += uart.c uart_console.c
romstage-y	+= uart.c uart_console.c
ramstage-y	+= uart.c uart_console.c
endif

real-target: $(obj)/BOOT0

get_bootblock_size= \
	$(eval bb_s=$(shell $(CBFSTOOL) $(1) print | grep bootblocksize | \
					sed 's/[^0-9 ]//g')) \
	$(shell echo $$(($(word 2, $(strip $(bb_s))))))

# This tool is used to prepend a header to coreboot.rom to trick the SoC into
# loading out bootblock
#
MKSUNXIBOOT:=$(objutil)/mksunxiboot
$(MKSUNXIBOOT): $(top)/util/arm_boot_tools/mksunxiboot/mksunxiboot.c
	@printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

# The boot ROM in the SoC will start loading code if a special BOOT0 header is
# found (at an offset of 8KiB in either NAND or SD), and the checksum is
# correct. This header is added by the 'mxsunxiboot' tool, which is provided
# under util/arm_boot_tools/mksunxiboot. The boot ROM will load at most 24KiB of
# data to SRAM. The BOOT0 header takes 32 bytes, so bootblock is limited to
# 24KiB - 32 bytes.
# TODO: make mksunxiboot take the bootblock size as a parameter
# TODO: print an error if bootblock is too large (maybe place ROMSTAGE at the
#	exact offset needed to collide with the bootblock)
# FIXME: A10 loads 24KiB. According to Oliver other chips load a little more
#
$(obj)/BOOT0: $(obj)/coreboot.rom $(MKSUNXIBOOT)
	@printf "    BOOT0      $(subst $(obj)/,,$(^))\n"
	$(MKSUNXIBOOT) $(word 1, $^) $@

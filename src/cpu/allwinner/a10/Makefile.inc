bootblock-y	+= clock.c
bootblock-y	+= raminit.c
bootblock-y	+= gpio.c
bootblock-y	+= pinmux.c
bootblock-y	+= timer.c
bootblock-y	+= bootblock_media.c
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += uart.c
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += uart_console.c

romstage-y	+= clock.c
romstage-y	+= uart.c
romstage-y	+= uart_console.c
romstage-y	+= bootblock_media.c
romstage-y	+= timer.c
romstage-y	+= mmc-generic.c

ramstage-y	+= clock.c
ramstage-y	+= uart.c
ramstage-y	+= uart_console.c
ramstage-y	+= timer.c
ramstage-y	+= monotonic_timer.c
ramstage-y	+= bootblock_media.c
ramstage-y	+= mmc-generic.c

real-target: $(obj)/BOOT0

get_bootblock_size= \
	$(eval bb_s=$(shell $(CBFSTOOL) $(1) print | grep bootblocksize | \
					sed 's/[^0-9 ]//g')) \
	$(shell echo $$(($(word 2, $(strip $(bb_s))))))

# The boot ROM in the SoC will start loading code if a special BOOT0 header is
# found (at an offset of 8KiB in either NAND or SD), and the checksum is
# correct. This header is normally added by the 'mxsunxiboot' tool. The boot ROM
# will load at most 24KiB of data to SRAM, so limit the file size accordingly.
# The BOOT0 header takes 32 bytes, so limit our file to 24KiB - 32 bytes.
# FIXME: Figure out how to safely integrate in coreboot.rom.
# FIXME: The file passed to mksunxiboot should only include the bootblock due
#	 to size limitations.
$(obj)/BOOT0: $(obj)/coreboot.rom
	@printf "    BOOT0      $(subst $(obj)/,,$(^))\n"
	touch $@
	dd if=$^ of=$^.tmp bs=24544 count=1
	-mksunxiboot $^.tmp $@

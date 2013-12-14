bootblock-y	+= pinmux.c
bootblock-y	+= bootblock_media.c
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += uart.c
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += uart_console.c

romstage-y	+= uart.c
romstage-y	+= uart_console.c
romstage-y	+= bootblock_media.c

ramstage-y	+= uart.c
ramstage-y	+= uart_console.c
ramstage-y	+= timer.c
ramstage-y	+= monotonic_timer.c
ramstage-y	+= bootblock_media.c


real-target: $(obj)/BOOT0

get_bootblock_size= \
	$(eval bb_s=$(shell $(CBFSTOOL) $(1) print | grep bootblocksize | \
					sed 's/[^0-9 ]//g')) \
	$(shell echo $$(($(word 2, $(strip $(bb_s))))))

# The boot ROM in the SoC will start loading code if a special boot0 header is
# found (at an offset of 8KiB in either NAND or SD), and the checksum is
# correct. this header is normally added by the 'mxsunxiboot' tool. The file
# passed to mksunxiboot should only include the bootblock due to size
# limitations.
# FIXME: Figure out how to safely integrate in coreboot.rom. For now, only copy
# the first 15 KiB of coreboot.rom (This will not collide with stack)
$(obj)/BOOT0: $(obj)/coreboot.rom
	@printf "    BOOT0      $(subst $(obj)/,,$(^))\n"
	touch $@
	dd if=$^ of=$^.tmp bs=1024 count=15
	-mksunxiboot $^.tmp $@

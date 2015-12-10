ifeq ($(CONFIG_SOC_MARVELL_ARMADA38X),y)

bootblock-y += bootblock.c
bootblock-y += bootblock_asm.S
bootblock-y += spi.c
bootblock-y += gpio.c
bootblock-y += monotonic_timer.c
bootblock-y += clock.c
ifeq ($(CONFIG_BOOTBLOCK_CONSOLE),y)
bootblock-$(CONFIG_DRIVERS_UART) += uart.c
endif

verstage-$(CONFIG_DRIVERS_UART) += uart.c
verstage-y += monotonic_timer.c
verstage-y += spi.c
verstage-y += gpio.c
verstage-y += i2c.c
verstage-y += clock.c

romstage-y += spi.c
romstage-y += gpio.c
romstage-y += cbmem.c
romstage-y += monotonic_timer.c
romstage-$(CONFIG_DRIVERS_UART) += uart.c
romstage-y += clock.c

ramstage-y += spi.c
ramstage-y += gpio.c
ramstage-y += cbmem.c
ramstage-y += monotonic_timer.c
ramstage-y += soc.c
ramstage-$(CONFIG_DRIVERS_UART) += uart.c
ramstage-y += clock.c

CPPFLAGS_common += -Isrc/soc/marvell/armada38x/include/ -Isrc/commonlib/include/commonlib/

BIN_HDR = 3rdparty/cpu/marvell/armada38x/bin_hdr.bin
DOIMAGE = $(objutil)/marvell/doimage_mv/doimage

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin $(DOIMAGE)
	@chmod a+x $(DOIMAGE)
	$(DOIMAGE) -T flash -D 0 -E 0 -G $(BIN_HDR) $< $@
	rm $<
endif

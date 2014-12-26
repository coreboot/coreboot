bootblock-y += spi.c alternate_cbfs.c
bootblock-y += bootblock.c
bootblock-y += pinmux.c mct.c power.c
# Clock is required for UART
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += clock_init.c
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += clock.c
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += monotonic_timer.c
ifeq ($(CONFIG_DRIVERS_UART),y)
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += uart.c
endif
bootblock-y += wakeup.c
bootblock-y += gpio.c
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += timer.c

romstage-y += spi.c alternate_cbfs.c
romstage-y += clock.c
romstage-y += clock_init.c
romstage-y += pinmux.c  # required by s3c24x0_i2c and uart.
romstage-y += dmc_common.c
romstage-y += dmc_init_ddr3.c
romstage-y += power.c
romstage-y += mct.c
romstage-y += monotonic_timer.c
romstage-$(CONFIG_DRIVERS_UART) += uart.c
romstage-y += wakeup.c
romstage-y += gpio.c
romstage-y += timer.c
romstage-y += trustzone.c
romstage-y += i2c.c
#romstage-y += wdt.c
romstage-y += cbmem.c

ramstage-y += spi.c alternate_cbfs.c
ramstage-y += clock.c
ramstage-y += clock_init.c
ramstage-y += pinmux.c
ramstage-y += power.c
ramstage-$(CONFIG_DRIVERS_UART) += uart.c
ramstage-y += cpu.c
ramstage-y += tmu.c
ramstage-y += mct.c
ramstage-y += monotonic_timer.c
ramstage-y += timer.c
ramstage-y += gpio.c
ramstage-y += i2c.c
ramstage-y += dp-reg.c
ramstage-y += fb.c
ramstage-y += usb.c
ramstage-y += cbmem.c

$(objcbfs)/bootblock.raw.elf: $(objcbfs)/bootblock.elf
	cp $< $@

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	@printf "    BL1, CKSUM $(subst $(obj)/,,$(@))\n"
	util/exynos/fixed_cksum.py $< $<.cksum 32768
	cat 3rdparty/cpu/samsung/exynos5250/bl1.bin $<.cksum > $@

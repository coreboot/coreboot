## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_NVIDIA_TEGRA124),y)

bootblock-y += bootblock.c
bootblock-y += bootblock_asm.S
bootblock-y += clock.c
bootblock-y += dma.c
bootblock-y += i2c.c
bootblock-y += dma.c
bootblock-y += maincpu.S
bootblock-y += monotonic_timer.c
bootblock-y += power.c
bootblock-y += spi.c
bootblock-y += ../tegra/gpio.c
bootblock-y += ../tegra/i2c.c
bootblock-$(CONFIG_SOFTWARE_I2C) += ../tegra/software_i2c.c
bootblock-y += ../tegra/pingroup.c
bootblock-y += ../tegra/pinmux.c
bootblock-y += ../tegra/apbmisc.c
bootblock-y += uart.c

verstage-y += verstage.c
verstage-y += dma.c
verstage-y += monotonic_timer.c
verstage-y += spi.c
verstage-y += uart.c
verstage-y += ../tegra/gpio.c
verstage-y += ../tegra/i2c.c
verstage-y += ../tegra/pinmux.c
verstage-y += clock.c
verstage-y += i2c.c
verstage-y += cache.c

romstage-y += cbmem.c
romstage-y += clock.c
romstage-y += dma.c
romstage-y += i2c.c
romstage-y += monotonic_timer.c
romstage-y += power.c
romstage-y += sdram.c
romstage-y += sdram_lp0.c
romstage-y += spi.c
romstage-y += ../tegra/gpio.c
romstage-y += ../tegra/i2c.c
romstage-$(CONFIG_SOFTWARE_I2C) += ../tegra/software_i2c.c
romstage-y += ../tegra/pinmux.c
romstage-y += cache.c
romstage-y += uart.c

ramstage-y += clock.c
ramstage-y += display.c
ramstage-y += dma.c
ramstage-y += i2c.c
ramstage-y += maincpu.S
ramstage-y += monotonic_timer.c
ramstage-y += sdram.c
ramstage-y += soc.c
ramstage-y += sor.c
ramstage-y += spi.c
ramstage-y += dp.c
ramstage-y += ../tegra/gpio.c
ramstage-y += ../tegra/i2c.c
ramstage-$(CONFIG_SOFTWARE_I2C) += ../tegra/software_i2c.c
ramstage-y += ../tegra/pinmux.c
ramstage-y += ../tegra/usb.c
ramstage-y += uart.c

rmodules_$(ARCH-romstage-y)-y += monotonic_timer.c

CPPFLAGS_common += -Isrc/soc/nvidia/tegra124/include/

# We want to grab the bootblock right before it goes into the image and wrap
# it inside a BCT, but ideally we would do that without making special, one
# use modifications to the main ARM Makefile. We do this in two ways. First,
# we copy bootblock.elf to bootblock.raw.elf and allow the %.bin: %.elf
# template rule to turn it into bootblock.raw.bin. This makes sure whatever
# processing is supposed to happen to turn an .elf into a .bin happens.
#
# Second, we add our own rule for creating bootblock.bin from
# bootblock.raw.bin which displaces the template rule. When other rules that
# package up the image pull in bootblock.bin, it will be this wrapped version
# instead of the raw bootblock.

$(obj)/generated/bct.bin: $(obj)/generated/bct.cfg $(CBOOTIMAGE)
	@printf "    CBOOTIMAGE $(subst $(obj)/,,$(@))\n"
	$(CBOOTIMAGE) -gbct --soc tegra124 $< $@

BCT_BIN = $(obj)/generated/bct.bin
BCT_WRAPPER = $(obj)/generated/bct.wrapper
$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin $(BCT_BIN) $(CBOOTIMAGE)
	echo "Version    = 1;" > $(BCT_WRAPPER)
	echo "Redundancy = 1;" >> $(BCT_WRAPPER)
	echo "Bctfile    = $(BCT_BIN);" >> $(BCT_WRAPPER)
	echo "BootLoader = $<,$(call loadaddr,bootblock),$(call loadaddr,bootblock),Complete;" >> $(BCT_WRAPPER)
	@printf "    CBOOTIMAGE $(subst $(obj)/,,$(@))\n"
	$(CBOOTIMAGE) $(BCT_WRAPPER) $@

endif

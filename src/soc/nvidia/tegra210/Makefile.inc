## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_NVIDIA_TEGRA210),y)

CBOOTIMAGE_OPTS = --soc tegra210

bootblock-y += bootblock.c
bootblock-y += bootblock_asm.S
bootblock-y += clock.c
bootblock-y += spi.c
bootblock-y += i2c.c
bootblock-y += dma.c
bootblock-y += monotonic_timer.c
bootblock-y += padconfig.c
bootblock-y += power.c
bootblock-y += funitcfg.c
bootblock-y += ../tegra/gpio.c
bootblock-y += ../tegra/i2c.c
bootblock-y += ../tegra/pingroup.c
bootblock-y += ../tegra/pinmux.c
bootblock-y += ../tegra/apbmisc.c
bootblock-y += ../tegra/usb.c
bootblock-y += uart.c

verstage-y += dma.c
verstage-y += monotonic_timer.c
verstage-y += spi.c
verstage-y += padconfig.c
verstage-y += funitcfg.c
verstage-y += uart.c
verstage-y += ../tegra/gpio.c
verstage-y += ../tegra/i2c.c
verstage-y += ../tegra/pinmux.c
verstage-y += clock.c
verstage-y += i2c.c

romstage-y += romstage_asm.S
romstage-y += addressmap.c
romstage-y += cbmem.c
romstage-y += ccplex.c
romstage-y += clock.c
romstage-y += cpu.c
romstage-y += spi.c
romstage-y += i2c.c
romstage-y += dma.c
romstage-y += monotonic_timer.c
romstage-y += padconfig.c
romstage-y += funitcfg.c
romstage-y += romstage.c
romstage-y += power.c
romstage-y += ram_code.c
ifneq ($(CONFIG_BOOTROM_SDRAM_INIT),y)
romstage-y += sdram.c
romstage-y += sdram_lp0.c
endif
romstage-y += ../tegra/gpio.c
romstage-y += ../tegra/i2c.c
romstage-y += ../tegra/pinmux.c
romstage-y += ../tegra/usb.c
romstage-y += uart.c

ramstage-y += addressmap.c
ramstage-y += cbmem.c
ramstage-y += cpu.c
ramstage-y += clock.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += dc.c
ramstage-$(CONFIG_MAINBOARD_DO_DSI_INIT) += dsi.c
ramstage-$(CONFIG_MAINBOARD_DO_DSI_INIT) += mipi_dsi.c
ramstage-$(CONFIG_MAINBOARD_DO_DSI_INIT) += mipi.c
ramstage-$(CONFIG_MAINBOARD_DO_DSI_INIT) += mipi-phy.c
ramstage-$(CONFIG_MAINBOARD_DO_DSI_INIT) += ./jdi_25x18_display/panel-jdi-lpm102a188a.c
ramstage-$(CONFIG_MAINBOARD_DO_SOR_INIT) += dp.c
ramstage-$(CONFIG_MAINBOARD_DO_SOR_INIT) += sor.c
ramstage-$(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE) += arm_tf.c

ramstage-y += sdram_lp0.c
ramstage-y += soc.c
ramstage-y += spi.c
ramstage-y += i2c.c
ramstage-y += i2c6.c
ramstage-y += ape.c
ramstage-y += power.c
ramstage-y += dma.c
ramstage-y += monotonic_timer.c
ramstage-y += padconfig.c
ramstage-y += funitcfg.c
ramstage-y += ram_code.c
ramstage-y += ../tegra/apbmisc.c
ramstage-y += ../tegra/gpio.c
ramstage-y += ../tegra/i2c.c
ramstage-y += ../tegra/pinmux.c
ramstage-y += ramstage.c
ramstage-y += mmu_operations.c
ramstage-y += uart.c
ramstage-y += ../tegra/usb.c
ramstage-$(CONFIG_HAVE_MTC) += mtc.c
ramstage-y += stage_entry.S

rmodules_arm-y += monotonic_timer.c

CPPFLAGS_common += -Isrc/soc/nvidia/tegra210/include/

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

$(objcbfs)/bootblock.raw.elf: $(objcbfs)/bootblock.elf
	cp $< $@

$(obj)/generated/bct.bin: $(obj)/generated/bct.cfg $(CBOOTIMAGE)
	@printf "    CBOOTIMAGE $(subst $(obj)/,,$(@))\n"
	$(CBOOTIMAGE) -gbct $(CBOOTIMAGE_OPTS) $< $@

BCT_BIN = $(obj)/generated/bct.bin
BCT_WRAPPER = $(obj)/generated/bct.wrapper
$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin $(BCT_BIN)
	echo "Version    = 1;" > $(BCT_WRAPPER)
	echo "Redundancy = 1;" >> $(BCT_WRAPPER)
	echo "Bctcopy    = 1;" >> $(BCT_WRAPPER)
	echo "Bctfile    = $(BCT_BIN);" >> $(BCT_WRAPPER)
	echo "BootLoader = $<,$(call loadaddr,bootblock),$(call loadaddr,bootblock),Complete;" >> $(BCT_WRAPPER)
	@printf "    CBOOTIMAGE $(subst $(obj)/,,$(@))\n"
	$(CBOOTIMAGE) $(CBOOTIMAGE_OPTS) $(BCT_WRAPPER) $@

# We need to ensure that TZ memory has enough space to hold TTB and resident EL3
# components (including BL31 and Secure OS)
ttb_size=$(shell printf "%d" $(CONFIG_TTB_SIZE_MB))
sec_size=$(shell printf "%d" $(CONFIG_SEC_COMPONENT_SIZE_MB))
req_tz_size=$(shell expr $(ttb_size) + $(sec_size))

tz_size=$(shell printf "%d" $(CONFIG_TRUSTZONE_CARVEOUT_SIZE_MB))

ifeq ($(shell test $(tz_size) -lt $(req_tz_size) && echo 1), 1)
     $(error "TRUSTZONE_CARVEOUT_SIZE_MB should be at least as big as TTB_SIZE_MB + SEC_COMPONENT_SIZE_MB")
endif

# BL31 component is placed towards the end of 32-bit address space. This assumes
# that TrustZone memory is placed at the end of 32-bit address space. Within the
# TZ memory, we place BL31 and BL32(if available) towards the beginning and TTB
# towards the end. Calculate TZDRAM_BASE i.e. base of BL31 component by:
#   0x1000 = end of 32-bit address space in MiB
#   0x1000 - $(CONFIG_TRUSTZONE_CARVEOUT_SIZE_MB) = start of TZ memory in MiB
BL31_MAKEARGS += TZDRAM_BASE=$$(((0x1000 - $(CONFIG_TRUSTZONE_CARVEOUT_SIZE_MB)) << 20))
BL31_MAKEARGS += PLAT=tegra TARGET_SOC=t210

# MTC fw
MTC_DIR = $(CONFIG_MTC_DIRECTORY)
MTC_FILE = $(MTC_DIR)/$(CONFIG_MTC_FILE)
MTC_FILE_CBFS = $(CONFIG_MTC_FILE)
cbfs-files-$(CONFIG_HAVE_MTC) += $(MTC_FILE_CBFS)
$(MTC_FILE_CBFS)-file := $(MTC_FILE)
$(MTC_FILE_CBFS)-type := raw

endif

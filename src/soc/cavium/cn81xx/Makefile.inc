## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_CAVIUM_CN81XX),y)

# bootblock
bootblock-$(CONFIG_BOOTBLOCK_CUSTOM) += bootblock_custom.S
bootblock-y += bootblock.c
bootblock-y += twsi.c
bootblock-y += clock.c
bootblock-y += gpio.c
bootblock-y += timer.c
bootblock-y += spi.c
bootblock-y += uart.c
bootblock-y += cpu.c

################################################################################
# verstage

verstage-y += twsi.c
verstage-y += clock.c
verstage-y += gpio.c
verstage-y += timer.c
verstage-y += spi.c
verstage-y += uart.c

################################################################################
# romstage

romstage-y += twsi.c
romstage-y += clock.c
romstage-y += gpio.c
romstage-y += timer.c
romstage-y += spi.c
romstage-y += uart.c
romstage-y += cbmem.c

romstage-y += sdram.c
romstage-y += mmu.c

################################################################################
# ramstage

ramstage-y += twsi.c
ramstage-y += clock.c
ramstage-y += gpio.c
ramstage-y += timer.c
ramstage-y += spi.c
ramstage-y += uart.c
ramstage-y += sdram.c
ramstage-y += soc.c
ramstage-y += cpu.c
ramstage-y += cpu_secondary.S
ramstage-y += ecam0.c

ramstage-$(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE) += bl31_plat_params.c

BL31_MAKEARGS += PLAT=t81 M0_CROSS_COMPILE="$(CROSS_COMPILE_arm)" ENABLE_SPE_FOR_LOWER_ELS=0

CPPFLAGS_common += -Isrc/soc/cavium/cn81xx/include

endif

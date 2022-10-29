## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_QUALCOMM_QCS405),y)

################################################################################
all-y += clock.c
all-y += spi.c
all-y += ../common/timer.c
all-y += gpio.c
all-y += i2c.c
all-y += qup.c
all-y += blsp.c
all-$(CONFIG_DRIVERS_UART) += uart.c

################################################################################
bootblock-y += bootblock.c
bootblock-y += mmu.c

################################################################################
romstage-y += cbmem.c
romstage-y += usb.c

################################################################################
ramstage-y += soc.c
ramstage-y += usb.c

################################################################################

CPPFLAGS_common += -Isrc/soc/qualcomm/qcs405/include

endif

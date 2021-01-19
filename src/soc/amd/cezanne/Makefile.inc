# SPDX-License-Identifier: BSD-3-Clause

ifeq ($(CONFIG_SOC_AMD_CEZANNE),y)

subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr

# Beware that all-y also adds the compilation unit to verstage on PSP
all-y		+= config.c
all-y		+= aoac.c

bootblock-y	+= bootblock.c
bootblock-y	+= early_fch.c
bootblock-y	+= gpio.c
bootblock-y     += reset.c
bootblock-y	+= uart.c

verstage_x86-y	+= gpio.c
verstage_x86-y  += reset.c
verstage_x86-y	+= uart.c

romstage-y	+= gpio.c
romstage-y      += reset.c
romstage-y	+= romstage.c
romstage-y	+= uart.c

ramstage-y	+= chip.c
ramstage-y	+= gpio.c
ramstage-y      += reset.c
ramstage-y	+= uart.c

CPPFLAGS_common += -I$(src)/soc/amd/cezanne/include

endif

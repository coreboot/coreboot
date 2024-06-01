## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_NORTHBRIDGE_VIA_CX700),y)

bootblock-y	+= early_smbus.c bootblock.c
romstage-y	+= early_smbus.c romstage.c
ramstage-y	+= chip.c
all-y		+= clock.c reset.c

endif

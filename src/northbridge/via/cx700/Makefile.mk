## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_NORTHBRIDGE_VIA_CX700),y)

# raminit accesses addresses as low as 0
CFLAGS_bootblock += --param=min-pagesize=0
CFLAGS_romstage  += --param=min-pagesize=0

bootblock-y	+= early_smbus.c bootblock.c
romstage-y	+= early_smbus.c memmap.c romstage.c raminit.c
ramstage-y	+= memmap.c chip.c
all-y		+= clock.c reset.c

endif

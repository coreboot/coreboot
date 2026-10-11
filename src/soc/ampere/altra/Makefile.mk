## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_AMPERE_ALTRA),y)

all-y += atf.c
all-y += media.c
all-y += uart.c

bootblock-y += bootblock_custom.S
bootblock-y += cbmem.c

ramstage-y += soc.c

CFLAGS_common += -mcmodel=large

CPPFLAGS_common += -Isrc/soc/ampere/altra/include
CPPFLAGS_common += -mstrict-align

endif

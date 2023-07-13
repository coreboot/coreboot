## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_GENOA),y)

bootblock-y	+= early_fch.c

romstage-y	+= romstage.c

ramstage-y	+= chip.c
ramstage-y	+= timer.c

CPPFLAGS_common += -I$(src)/soc/amd/genoa/include

ifeq ($(call int-gt, $(CONFIG_ROM_SIZE) 0x1000000), 1)
CBFSTOOL_ADD_CMD_OPTIONS+= --mmap 0:0xff000000:0x1000000
endif

endif

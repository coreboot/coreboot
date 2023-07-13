## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_GENOA),y)

bootblock-y	+= early_fch.c

romstage-y	+= romstage.c

ramstage-y	+= chip.c
ramstage-y	+= timer.c

CPPFLAGS_common += -I$(src)/soc/amd/genoa/include

endif

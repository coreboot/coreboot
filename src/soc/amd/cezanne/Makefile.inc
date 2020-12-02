# SPDX-License-Identifier: BSD-3-Clause

ifeq ($(CONFIG_SOC_AMD_CEZANNE),y)

bootblock-y	+= bootblock.c

romstage-y	+= romstage.c

ramstage-y	+= chip.c
ramstage-y	+= timer.c

CPPFLAGS_common += -I$(src)/soc/amd/cezanne/include

endif

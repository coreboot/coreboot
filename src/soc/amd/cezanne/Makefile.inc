# SPDX-License-Identifier: BSD-3-Clause

ifeq ($(CONFIG_SOC_AMD_CEZANNE),y)

# Beware that all-y also adds the compilation unit to verstage on PSP
all-y += config.c

bootblock-y	+= bootblock.c
bootblock-y	+= early_fch.c

romstage-y	+= romstage.c

ramstage-y	+= chip.c

CPPFLAGS_common += -I$(src)/soc/amd/cezanne/include

endif

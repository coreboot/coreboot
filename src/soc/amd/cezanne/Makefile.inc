# SPDX-License-Identifier: BSD-3-Clause

ifeq ($(CONFIG_SOC_AMD_CEZANNE),y)

subdirs-y += ../../../cpu/x86/mtrr

# Beware that all-y also adds the compilation unit to verstage on PSP
all-y += config.c

bootblock-y	+= bootblock.c
bootblock-y	+= early_fch.c
bootblock-y     += reset.c

verstage_x86-y  += reset.c

romstage-y      += reset.c
romstage-y	+= romstage.c

ramstage-y	+= chip.c
ramstage-y      += reset.c

CPPFLAGS_common += -I$(src)/soc/amd/cezanne/include

endif

## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_QUALCOMM_DOUGIE),y)

decompressor-y += decompressor.c
decompressor-y += mmu.c
decompressor-y += ../common/timer.c
all-y += ../common/timer.c
all-y += ../common/gpio.c
all-y += ../common/clock.c
all-y += clock.c

################################################################################
bootblock-y += bootblock.c
bootblock-y += mmu.c

################################################################################
romstage-y += cbmem.c

################################################################################
ramstage-y += soc.c
ramstage-y += cbmem.c

################################################################################

CPPFLAGS_common += -Isrc/soc/qualcomm/dougie/include
CPPFLAGS_common += -Isrc/soc/qualcomm/common/include

endif

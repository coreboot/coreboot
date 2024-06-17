## SPDX-License-Identifier: GPL-2.0-only

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

bootblock-y += bootblock.c
bootblock-y += superio.c

romstage-y += romstage.c

ramstage-y += ramstage.c
ramstage-y += gpio.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_HDA_VERB) += hda_verb.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads

all-y += beep.c
all-y += panic.c

smm-y += beep.c
smm-y += panic.c

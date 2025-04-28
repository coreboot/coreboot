# SPDX-License-Identifier: GPL-2.0-only
bootblock-y += gpio.c

romstage-y += gpio.c
ramstage-y += gpio.c

ramstage-y += variant.c

ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_HDA_VERB) += hda_verb.c

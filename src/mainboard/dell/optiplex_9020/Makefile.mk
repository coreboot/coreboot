## SPDX-License-Identifier: GPL-2.0-only

romstage-y += gpio.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
ramstage-y += sch5555_ec.c
bootblock-y += bootblock.c sch5555_ec.c

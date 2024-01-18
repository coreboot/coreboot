# SPDX-License-Identifier: GPL-2.0-or-later

romstage-y += gpio.c
romstage-y += pei_data.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
ramstage-y += pei_data.c

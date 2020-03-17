##
## This file is part of the coreboot project.
##
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

smm-y += smihandler.c
bootblock-y += variants/$(VARIANT_DIR)/gpio.c
romstage-y += variants/$(VARIANT_DIR)/gpio.c
romstage-y += variants/$(VARIANT_DIR)/romstage.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/$(VARIANT_DIR)/gma-mainboard.ads
subdirs-$(CONFIG_BOARD_LENOVO_T431S) += variants/$(VARIANT_DIR)/spd

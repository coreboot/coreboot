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

romstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC) += ec.c
ramstage-y += irqroute.c
ramstage-y += w25q64.c

smm-y += mainboard_smi.c

ramstage-y += variants/$(VARIANT_DIR)/gpio.c

ramstage-$(CONFIG_BOARD_GOOGLE_NINJA) += variants/$(VARIANT_DIR)/lan.c
ramstage-$(CONFIG_BOARD_GOOGLE_SUMO) += variants/$(VARIANT_DIR)/lan.c

subdirs-y += variants/$(VARIANT_DIR)

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

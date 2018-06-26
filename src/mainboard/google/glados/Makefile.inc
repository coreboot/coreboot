##
## This file is part of the coreboot project.
##
## Copyright (C) 2015 Google Inc.
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

subdirs-y += spd

bootblock-y += bootblock_mainboard.c

romstage-y += spd/spd.c

bootblock-$(CONFIG_CHROMEOS) += chromeos.c
verstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c

ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC) += ec.c

ramstage-y += mainboard.c
ramstage-y += ramstage.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

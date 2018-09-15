##
## This file is part of the coreboot project.
##
## Copyright (C) 2014 Google Inc.
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

ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC) += ec.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c

romstage-y += variants/$(VARIANT_DIR)/pei_data.c
ramstage-y += variants/$(VARIANT_DIR)/pei_data.c

ifneq ($(CONFIG_BOARD_GOOGLE_BUDDY),y)
romstage-y += variants/$(VARIANT_DIR)/variant.c
endif
ramstage-y += variants/$(VARIANT_DIR)/variant.c

subdirs-y += variants/$(VARIANT_DIR)
subdirs-y += variants/$(VARIANT_DIR)/spd

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

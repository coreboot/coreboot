##
## This file is part of the coreboot project.
##
## Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

bootblock-y += variants/$(VARIANT_DIR)/early_init.c
romstage-y  += variants/$(VARIANT_DIR)/early_init.c
bootblock-y += variants/$(VARIANT_DIR)/gpio.c
romstage-y  += variants/$(VARIANT_DIR)/gpio.c
ramstage-y  += variants/$(VARIANT_DIR)/hda_verb.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/$(VARIANT_DIR)/gma-mainboard.ads

# FIXME: Other variants with same size onboard RAM may exist.
SPD_SOURCES = hynix_4g

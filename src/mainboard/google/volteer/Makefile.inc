##
## This file is part of the coreboot project.
##
## Copyright 2020 The coreboot project Authors.
##
## SPDX-License-Identifier: GPL-2.0-or-later
##

bootblock-y += bootblock.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-y += romstage.c

ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-y += ec.c
ramstage-y += mainboard.c

smm-y += smihandler.c

verstage-$(CONFIG_CHROMEOS) += chromeos.c

subdirs-y += variants/baseboard
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include

VARIANT_DIR:=$(call strip_quotes,$(CONFIG_VARIANT_DIR))
subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

subdirs-y += spd

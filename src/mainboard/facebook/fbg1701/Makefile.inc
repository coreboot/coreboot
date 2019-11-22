##
## This file is part of the coreboot project.
##
## Copyright (C) 2013 Google Inc.
## Copyright (C) 2015 Intel Corp.
## Copyright (C) 2018-2019 Eltan B.V.
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

ifneq ($(filter y,$(CONFIG_VENDORCODE_ELTAN_VBOOT) $(CONFIG_VENDORCODE_ELTAN_MBOOT)),)
bootblock-y += board_verified_boot.c
postcar-y += board_verified_boot.c
ramstage-y += board_verified_boot.c
romstage-y += board_verified_boot.c
endif

bootblock-y += com_init.c

ramstage-y += cpld.c
ramstage-y += gpio.c
ramstage-y += hda_verb.c
ramstage-y += irqroute.c
ramstage-y += ramstage.c
ramstage-y += w25q64.c

romstage-y += cpld.c

# Order of names in SPD_SOURCES is important!
SPD_SOURCES  = SAMSUNG_K4B8G1646D-MYKO
SPD_SOURCES += MICRON_MT41K512M16HA-125A
SPD_SOURCES += KINGSTON_B5116ECMDXGGB

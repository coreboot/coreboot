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

subdirs-$(CONFIG_ENABLE_FSP_MEMORY_DOWN) += spd

romstage-y += boardid.c
romstage-y += hsio.c

ramstage-y += ramstage.c
ramstage-y += boardid.c
ramstage-y += hsio.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c

CPPFLAGS_common += -Isrc/mainboard/$(MAINBOARDDIR)/

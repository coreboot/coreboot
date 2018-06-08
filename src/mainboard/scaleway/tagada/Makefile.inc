##
## This file is part of the coreboot project.
##
## Copyright (C) 2014 - 2017 Intel Corporation.
## Copyright (C) 2017 Online SAS.
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

bootblock-y += bootblock.c

romstage-y += hsio.c

ramstage-y += ramstage.c
ramstage-y += hsio.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi_tables.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c

bootblock-y += bmcinfo.c
postcar-y += bmcinfo.c
romstage-y += bmcinfo.c
ramstage-y += bmcinfo.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += bmcinfo.c

CPPFLAGS_common += -Isrc/mainboard/$(MAINBOARDDIR)/

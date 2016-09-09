##
## This file is part of the coreboot project.
##
## Copyright (C) 2016 Marvell, Inc.
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
bootblock-y += memlayout.ld
bootblock-y += reset.c
bootblock-$(CONFIG_CHROMEOS) += chromeos.c

ramstage-y += mainboard.c
ramstage-y += memlayout.ld
ramstage-y += reset.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c

romstage-y += memlayout.ld
romstage-y += reset.c
romstage-y += dram_params.c
romstage-$(CONFIG_CHROMEOS) += chromeos.c

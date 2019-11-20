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

bootblock-y += memlayout.ld
bootblock-y += boardid.c
bootblock-y += chromeos.c
bootblock-y += bootblock.c

verstage-y += memlayout.ld
ifneq ($(CONFIG_BOARD_GOOGLE_BUBS),y)
verstage-y += reset.c
endif
verstage-y += boardid.c
verstage-y += chromeos.c

romstage-y += memlayout.ld
romstage-y += romstage.c
romstage-y += boardid.c
romstage-y += chromeos.c

ramstage-y += memlayout.ld
ramstage-y += mainboard.c
ifneq ($(CONFIG_BOARD_GOOGLE_BUBS),y)
ramstage-y += reset.c
endif
ramstage-y += chromeos.c
ramstage-y += boardid.c

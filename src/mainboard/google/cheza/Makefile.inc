##
## This file is part of the coreboot project.
##
## Copyright 2018 Google LLC
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

bootblock-y += boardid.c
bootblock-y += memlayout.ld
bootblock-y += chromeos.c
bootblock-y += bootblock.c

verstage-y += boardid.c
verstage-y += memlayout.ld
verstage-y += chromeos.c

romstage-y += boardid.c
romstage-y += memlayout.ld
romstage-y += chromeos.c
romstage-y += romstage.c

ramstage-y += boardid.c
ramstage-y += memlayout.ld
ramstage-y += chromeos.c
ramstage-y += mainboard.c

##
## This file is part of the coreboot project.
##
## Copyright 2012 Google Inc.
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

bootblock-y += wakeup.c

romstage-y += memory.c
romstage-y += romstage.c
romstage-y += wakeup.c
romstage-y += chromeos.c

ramstage-y += mainboard.c
ramstage-y += chromeos.c

bootblock-y += memlayout.ld
romstage-y += memlayout.ld
ramstage-y += memlayout.ld

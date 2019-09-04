##
## This file is part of the coreboot project.
##
## Copyright (C) 2013 Google Inc.
##
## This software is licensed under the terms of the GNU General Public
## License version 2, as published by the Free Software Foundation, and
## may be copied, distributed, and modified under those terms.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.

bootblock-y += uart.c
bootblock-y += rom_media.c
bootblock-y += clint.c

romstage-y += romstage.c
romstage-y += uart.c
romstage-y += rom_media.c
romstage-y += clint.c

ramstage-y += uart.c
ramstage-y += rom_media.c
ramstage-y += clint.c

bootblock-y += memlayout.ld
romstage-y += memlayout.ld
ramstage-y += memlayout.ld

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

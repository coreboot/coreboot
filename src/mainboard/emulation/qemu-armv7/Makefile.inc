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

romstage-y += romstage.c

romstage-y += cbmem.c
ramstage-y += cbmem.c

bootblock-y += media.c
romstage-y += media.c
ramstage-y += media.c

bootblock-y += timer.c
romstage-y += timer.c
ramstage-y += timer.c

bootblock-y += mmio.c
romstage-y += mmio.c
ramstage-y += mmio.c

bootblock-y += memlayout.ld
romstage-y += memlayout.ld
ramstage-y += memlayout.ld

##
## This file is part of the coreboot project.
##
##
## This software is licensed under the terms of the GNU General Public
## License version 2, as published by the Free Software Foundation, and
## may be copied, distributed, and modified under those terms.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.

bootblock-y += bootblock.c
bootblock-y += uart.c
romstage-y += cbmem.c
romstage-y += romstage.c
ramstage-y += timer.c
romstage-y += uart.c
ramstage-y += uart.c

bootblock-y += memlayout.ld
romstage-y += memlayout.ld
ramstage-y += memlayout.ld

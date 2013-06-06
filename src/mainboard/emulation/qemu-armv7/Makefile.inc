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

bootblock-y += media.c
romstage-y += media.c
ramstage-y += media.c

bootblock-y += timer.c
romstage-y += timer.c
ramstage-y += timer.c

bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += uart.c
romstage-$(CONFIG_EARLY_CONSOLE) += uart.c
ramstage-y += uart.c

##
## This file is part of the coreboot project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

driver-y += i82801ax.c
driver-y += i82801ax_ac97.c
driver-y += i82801ax_ide.c
driver-y += i82801ax_lpc.c
driver-y += i82801ax_pci.c
driver-y += i82801ax_smbus.c
driver-y += i82801ax_usb.c

ramstage-y += i82801ax_reset.c
ramstage-y += i82801ax_watchdog.c

romstage-y += i82801ax_early_smbus.c


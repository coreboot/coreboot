##
## This file is part of the coreboot project.
##
## Copyright (C) 2011  Alexandru Gagniuc <mr.nuke.me@gmail.com>
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

romstage-y += early_smbus.c
romstage-y += raminit_ddr3.c
romstage-y += raminit_ddr3_delay_calib.c
romstage-y += ./../../../devices/dram/dram_util.c
romstage-y += ./../../../devices/smbus/early_smbus.c
#romstage-y += ./../../../cpu/x86/lapic/apic_timer.c

# Drivers for these devices already exist with the vx800
# Use those instead of duplicating code

driver-y += ./../vx800/northbridge.c
driver-y += ./../vx800/vga.c
driver-y += ./../vx800/lpc.c

chipset_bootblock_inc += $(src)/northbridge/via/vx900/romstrap.inc
chipset_bootblock_lds += $(src)/northbridge/via/vx900/romstrap.lds

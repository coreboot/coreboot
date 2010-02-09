##
## This file is part of the coreboot project.
##
## Copyright (C) 2008 VIA Technologies, Inc.
## (Written by Aaron Lwe <aaron.lwe@gmail.com> for VIA)
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

initobj-y += crt0.o
obj-y += mainboard.o
obj-y += vgabios.o
obj-$(CONFIG_GENERATE_PIRQ_TABLE) += irq_tables.o

obj-$(CONFIG_GENERATE_ACPI_TABLES) += fadt.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += dsdt.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += acpi_tables.o

ldscripts := $(src)/arch/i386/init/ldscript_fallback_cbfs.lb
ldscripts += $(src)/cpu/x86/16bit/entry16.lds
ldscripts += $(src)/northbridge/via/vx800/romstrap.lds
ldscripts += $(src)/cpu/x86/16bit/reset16.lds
ldscripts += $(src)/arch/i386/lib/id.lds
ldscripts += $(src)/arch/i386/lib/failover.lds

crt0s := $(src)/cpu/x86/16bit/entry16.inc
crt0s += $(src)/cpu/x86/32bit/entry32.inc
crt0s += $(src)/cpu/x86/16bit/reset16.inc
crt0s += $(src)/northbridge/via/vx800/romstrap.inc
crt0s += $(src)/arch/i386/lib/id.inc
crt0s += $(src)/cpu/x86/fpu_enable.inc
crt0s += $(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc
crt0s += $(src)/cpu/x86/mmx_disable.inc


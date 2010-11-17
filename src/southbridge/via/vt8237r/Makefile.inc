##
## This file is part of the coreboot project.
##
## Copyright (C) 2007, 2009 Rudolf Marek <r.marek@assembler.cz>
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
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

driver-y += vt8237r.c
driver-y += vt8237_ctrl.c
driver-y += vt8237r_ide.c
driver-y += vt8237r_lpc.c
driver-y += vt8237r_sata.c
driver-y += vt8237r_usb.c
driver-$(CONFIG_PIRQ_ROUTE) += vt8237r_pirq.c
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += vt8237_fadt.c

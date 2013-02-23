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
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

ramstage-y += vt8237r.c
ramstage-y += ctrl.c
ramstage-y += ide.c
ramstage-y += lpc.c
ramstage-y += sata.c
ramstage-y += usb.c
ramstage-$(CONFIG_PIRQ_ROUTE) += pirq.c
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += fadt.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c

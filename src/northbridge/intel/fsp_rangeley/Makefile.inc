#
# This file is part of the coreboot project.
#
# Copyright (C) 2010 Google Inc.
# Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#

subdirs-y += fsp
ramstage-y += northbridge.c
ramstage-y += raminit.c

ramstage-y += acpi.c
ramstage-y += port_access.c

romstage-y += raminit.c
romstage-y += ../../../arch/x86/lib/walkcbfs.S
romstage-y += port_access.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += udelay.c

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)

CPPFLAGS_common += -I$(src)/northbridge/intel/fsp_rangeley/
CPPFLAGS_common += -I$(src)/northbridge/intel/fsp_rangeley/fsp


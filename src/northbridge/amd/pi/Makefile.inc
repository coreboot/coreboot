#
# This file is part of the coreboot project.
#
# Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
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

subdirs-$(CONFIG_NORTHBRIDGE_AMD_PI_00630F01) += 00630F01
subdirs-$(CONFIG_NORTHBRIDGE_AMD_PI_00730F01) += 00730F01

romstage-y += agesawrapper.c
romstage-y += def_callouts.c
ramstage-y += agesawrapper.c
ramstage-y += def_callouts.c

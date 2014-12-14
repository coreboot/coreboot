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
subdirs-$(CONFIG_NORTHBRIDGE_AMD_AGESA_FAMILY10) += family10
subdirs-$(CONFIG_NORTHBRIDGE_AMD_AGESA_FAMILY12) += family12
subdirs-$(CONFIG_NORTHBRIDGE_AMD_AGESA_FAMILY14) += family14
subdirs-$(CONFIG_NORTHBRIDGE_AMD_AGESA_FAMILY15) += family15
subdirs-$(CONFIG_NORTHBRIDGE_AMD_AGESA_FAMILY15_TN) += family15tn
subdirs-$(CONFIG_NORTHBRIDGE_AMD_AGESA_FAMILY15_RL) += family15rl
subdirs-$(CONFIG_NORTHBRIDGE_AMD_AGESA_FAMILY16_KB) += family16kb

romstage-y += def_callouts.c agesawrapper.c eventlog.c
ramstage-y += def_callouts.c agesawrapper.c eventlog.c

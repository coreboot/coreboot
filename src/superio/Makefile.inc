##
## This file is part of the coreboot project.
##
## Copyright (C) 2009 Ronald G. Minnich
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

subdirs-y += fintek
subdirs-y += intel
subdirs-y += ite
subdirs-y += nsc
subdirs-y += nuvoton
subdirs-y += renesas
#subdirs-y += serverengines
subdirs-y += smsc
subdirs-y += via
subdirs-y += winbond

ramstage-$(CONFIG_ARCH_X86) += common/conf_mode.c

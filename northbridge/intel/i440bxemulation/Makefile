##
## This file is part of the LinuxBIOS project.
##
## Copyright (C) 2006-2007 coresystems GmbH
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

# I think a .c.o rule might be more confusing than just putting the rule
# underneath the dependency graph -- but I welcome correction on this.

$(obj)/i440bx.o: $(src)/northbridge/intel/i440bxemulation/i440bx.c
	$(Q)$(CC) $(INITCFLAGS) -c $< -o $@

STAGE2_CHIPSET_OBJ += $(obj)/i440bx.o


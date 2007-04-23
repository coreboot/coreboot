##
## This file is part of the LinuxBIOS project.
##
## Copyright (C) 2007 coresystems GmbH
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

OPTIONS_OBJ := $(obj)/util/options/build_opt_tbl.o

$(obj)/util/options/build_opt_tbl: optionsdir $(OPTIONS_OBJ)
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -o $@ $(OPTIONS_OBJ)

optionsdir:
	$(Q)mkdir -p $(obj)/util/options/

$(obj)/util/options/%.o: $(src)/util/options/%.c
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -I$(src)/util/options -o $@ -c $<


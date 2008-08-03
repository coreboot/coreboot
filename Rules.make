##
## This file is part of the coreboot project.
##
## Copyright (C) 2006-2007 coresystems GmbH
## (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
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

#
# Miscellaneous mainboard specific build targets!
#

$(obj)/mainboard/$(MAINBOARDDIR)/statictree.o: $(obj)/mainboard/$(MAINBOARDDIR)/statictree.c $(obj)/statictree.h
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(INITCFLAGS) $(COREBOOTINCLUDE) -c -o $@ $<

$(obj)/mainboard/$(MAINBOARDDIR)/statictree.c: $(src)/mainboard/$(MAINBOARDDIR)/dts $(obj)/util/dtc/dtc
	$(Q)printf "  DTC     $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(obj)/util/dtc/dtc -O lb mainboard/$(MAINBOARDDIR)/dts > $@

$(obj)/statictree.h: $(src)/mainboard/$(MAINBOARDDIR)/dts $(obj)/util/dtc/dtc
	$(Q)printf "  DTC     $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(obj)/util/dtc/dtc -O lbh mainboard/$(MAINBOARDDIR)/dts > $@

$(obj)/mainboard/$(MAINBOARDDIR)/option_table.c: $(obj)/util/options/build_opt_tbl $(src)/mainboard/$(MAINBOARDDIR)/cmos.layout
	$(Q)printf "  OPTIONS $(subst $(shell pwd)/,,$(@))\n"
	$(Q)mkdir -p $(obj)/mainboard/$(MAINBOARDDIR)
	$(Q)$(obj)/util/options/build_opt_tbl -b \
		--config $(src)/mainboard/$(MAINBOARDDIR)/cmos.layout \
		--option $(obj)/mainboard/$(MAINBOARDDIR)/option_table.c 

$(obj)/mainboard/$(MAINBOARDDIR)/option_table.h: $(obj)/util/options/build_opt_tbl $(src)/mainboard/$(MAINBOARDDIR)/cmos.layout
	$(Q)printf "  OPTIONS $(subst $(shell pwd)/,,$(@))\n"
	$(Q)mkdir -p $(obj)/mainboard/$(MAINBOARDDIR)
	$(Q)$(obj)/util/options/build_opt_tbl -b \
		--config $(src)/mainboard/$(MAINBOARDDIR)/cmos.layout \
		--header $(obj)/mainboard/$(MAINBOARDDIR)/option_table.c 

$(obj)/option_table: $(obj)/mainboard/$(MAINBOARDDIR)/option_table.o
	$(Q)printf "  OBJCOPY $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(OBJCOPY) -O binary $< $@ 

$(obj)/mainboard/$(MAINBOARDDIR)/%.o: $(src)/mainboard/$(MAINBOARDDIR)/%.c
	$(Q)mkdir -p $(obj)/mainboard/$(MAINBOARDDIR)
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(INITCFLAGS) -c $< -o $@

#
# Miscellaneous superio specific build targets!
#

$(obj)/superio/%.o: $(src)/superio/%.c $(obj)/statictree.h
	$(Q)mkdir -p $(dir $@)
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(INITCFLAGS) -c $< -o $@

#
# Miscellaneous northbridge specific build targets!
#

$(obj)/northbridge/%.o: $(src)/northbridge/%.c $(obj)/statictree.h
	$(Q)mkdir -p $(dir $@)
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(INITCFLAGS) -c $< -o $@

#
# Miscellaneous southbridge specific build targets!
#

$(obj)/southbridge/%.o: $(src)/southbridge/%.c $(obj)/statictree.h
	$(Q)mkdir -p $(dir $@)
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(INITCFLAGS) -c $< -o $@


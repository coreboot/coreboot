##
## This file is part of the coreboot project.
##
## Copyright (C) 2007-2010 coresystems GmbH
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License as
## published by the Free Software Foundation; version 2 of
## the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
## MA 02110-1301 USA
##

##
## This mainboard requires DCACHE_AS_RAM enabled. It won't work without.
##

driver-y +=  mainboard.o

#needed by irq_tables and mptable and acpi_tables
obj-y += get_bus_conf.o
obj-$(CONFIG_GENERATE_MP_TABLE) += mptable.o
obj-$(CONFIG_GENERATE_PIRQ_TABLE) += irq_tables.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) +=  dsdt.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) +=  acpi_tables.o

# This is part of the conversion to init-obj and away from included code.

initobj-y += crt0.o

crt0s := $(src)/cpu/x86/16bit/entry16.inc
crt0s += $(src)/cpu/x86/32bit/entry32.inc
crt0s += $(src)/cpu/x86/16bit/reset16.inc
crt0s += $(src)/arch/i386/lib/id.inc
crt0s += $(src)/southbridge/nvidia/ck804/romstrap.inc
crt0s += $(src)/cpu/amd/car/cache_as_ram.inc
crt0s += $(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc

ldscripts := $(src)/arch/i386/init/ldscript_fallback_cbfs.lb
ldscripts += $(src)/cpu/x86/16bit/entry16.lds
ldscripts += $(src)/cpu/x86/16bit/reset16.lds
ldscripts += $(src)/arch/i386/lib/id.lds
ldscripts += $(src)/southbridge/nvidia/ck804/romstrap.lds
ldscripts += $(src)/arch/i386/lib/failover.lds

ifdef POST_EVALUATION

$(obj)/mainboard/$(MAINBOARDDIR)/dsdt.c: $(src)/mainboard/$(MAINBOARDDIR)/dsdt.dsl
	iasl -p $(obj)/mainboard/$(MAINBOARDDIR)/dsdt -tc $(src)/mainboard/$(MAINBOARDDIR)/dsdt.dsl
	mv $(obj)/mainboard/$(MAINBOARDDIR)/dsdt.hex $@

$(obj)/mainboard/$(MAINBOARDDIR)/dsdt.o: $(obj)/mainboard/$(MAINBOARDDIR)/dsdt.c
	$(CC) $(DISTRO_CFLAGS) $(CFLAGS) $(CPPFLAGS) $(DEBUG_CFLAGS) -I$(src) -I. -c $< -o $@

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc: $(src)/mainboard/$(MAINBOARDDIR)/romstage.c $(obj)/option_table.h
	$(CC) $(DISTRO_CFLAGS) $(CFLAGS) $(CPPFLAGS) $(DEBUG_CFLAGS) -I$(src) -I. -c -S  $(src)/mainboard/$(MAINBOARDDIR)/romstage.c -o $@
	perl -e 's/\.rodata/.rom.data/g' -pi $@
	perl -e 's/\.text/.section .rom.text/g' -pi $@

endif


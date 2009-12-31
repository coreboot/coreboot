##
## This file is part of the coreboot project.
##
## Copyright (C) 2007-2008 coresystems GmbH
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

driver-y += mainboard.o

# Needed by irq_tables and mptable and acpi_tables.
obj-y += get_bus_conf.o
obj-$(CONFIG_GENERATE_MP_TABLE) += mptable.o
obj-$(CONFIG_GENERATE_PIRQ_TABLE) += irq_tables.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += dsdt.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += acpi_tables.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += fadt.o

# ./ssdt.o is in northbridge/amd/amdfam10/Makefile.inc
obj-$(CONFIG_GENERATE_ACPI_TABLES) += ssdt2.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += ssdt3.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += ssdt4.o
obj-$(CONFIG_GENERATE_ACPI_TABLES) += ssdt5.o

driver-y += ../../../drivers/i2c/i2cmux2/i2cmux2.o

# This is part of the conversion to init-obj and away from included code.

initobj-y += crt0.o
# FIXME in $(top)/Makefile
crt0-y += ../../../../src/cpu/x86/32bit/entry32.inc
crt0-y += ../../../../src/cpu/amd/car/cache_as_ram.inc
crt0-y += auto.inc

ldscript-y += ../../../../src/arch/i386/init/ldscript_fallback_cbfs.lb
ldscript-y += ../../../../src/cpu/x86/32bit/entry32.lds
ldscript-y += ../../../../src/arch/i386/lib/failover.lds

ifdef POST_EVALUATION

$(obj)/mainboard/$(MAINBOARDDIR)/dsdt.c: $(src)/mainboard/$(MAINBOARDDIR)/dx/dsdt_lb.dsl
	iasl -p $(obj)/mainboard/$(MAINBOARDDIR)/dsdt -tc $<
	mv $(obj)/mainboard/$(MAINBOARDDIR)/dsdt.hex $@

$(obj)/mainboard/$(MAINBOARDDIR)/dsdt.o: $(obj)/mainboard/$(MAINBOARDDIR)/dsdt.c
	$(CC) $(DISTRO_CFLAGS) $(CFLAGS) $(CPPFLAGS) $(DEBUG_CFLAGS) -I$(src) -I. -c $< -o $@

$(obj)/mainboard/$(MAINBOARDDIR)/ssdt2.c: $(src)/mainboard/$(MAINBOARDDIR)/dx/pci2.asl
	iasl -p $(obj)/mainboard/$(MAINBOARDDIR)/pci2 -tc $<
	perl -pi -e 's/AmlCode/AmlCode_ssdt2/g' $(obj)/mainboard/$(MAINBOARDDIR)/pci2.hex
	mv $(obj)/mainboard/$(MAINBOARDDIR)/pci2.hex $@

$(obj)/mainboard/$(MAINBOARDDIR)/ssdt3.c: $(src)/mainboard/$(MAINBOARDDIR)/dx/pci3.asl
	iasl -p $(obj)/mainboard/$(MAINBOARDDIR)/pci3 -tc $<
	perl -pi -e 's/AmlCode/AmlCode_ssdt3/g' $(obj)/mainboard/$(MAINBOARDDIR)/pci3.hex
	mv $(obj)/mainboard/$(MAINBOARDDIR)/pci3.hex $@

$(obj)/mainboard/$(MAINBOARDDIR)/ssdt4.c: $(src)/mainboard/$(MAINBOARDDIR)/dx/pci4.asl
	iasl -p $(obj)/mainboard/$(MAINBOARDDIR)/pci4 -tc $<
	perl -pi -e 's/AmlCode/AmlCode_ssdt4/g' $(obj)/mainboard/$(MAINBOARDDIR)/pci4.hex
	mv $(obj)/mainboard/$(MAINBOARDDIR)/pci4.hex $@

$(obj)/mainboard/$(MAINBOARDDIR)/ssdt5.c: $(src)/mainboard/$(MAINBOARDDIR)/dx/pci5.asl
	iasl -p $(obj)/mainboard/$(MAINBOARDDIR)/pci5 -tc $<
	perl -pi -e 's/AmlCode/AmlCode_ssdt5/g' $(obj)/mainboard/$(MAINBOARDDIR)/pci5.hex
	mv $(obj)/mainboard/$(MAINBOARDDIR)/pci5.hex $@

$(obj)/mainboard/$(MAINBOARDDIR)/auto.inc: $(src)/mainboard/$(MAINBOARDDIR)/cache_as_ram_auto.c $(obj)/option_table.h
	$(CC) $(DISTRO_CFLAGS) $(CFLAGS) $(CPPFLAGS) $(DEBUG_CFLAGS) -I$(src) -I. -c -S  $(src)/mainboard/$(MAINBOARDDIR)/cache_as_ram_auto.c -o $@
	perl -e 's/\.rodata/.rom.data/g' -pi $@
	perl -e 's/\.text/.section .rom.text/g' -pi $@


endif


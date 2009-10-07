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

driver-y +=  mainboard.o
driver-y += ../../../drivers/i2c/i2cmux2/i2cmux2.o
driver-y += ../../../drivers/i2c/adm1027/adm1027.o

#needed by irq_tables and mptable and acpi_tables
obj-y += get_bus_conf.o
obj-$(CONFIG_HAVE_MP_TABLE) += mptable.o
obj-$(CONFIG_HAVE_PIRQ_TABLE) += irq_tables.o
obj-$(CONFIG_USE_INIT) += cache_as_ram_auto.o
obj-$(CONFIG_AP_CODE_IN_CAR) += apc_auto.o

# This is part of the conversion to init-obj and away from included code. 
initobj-y += crt0.o
crt0-y += ../../../../src/cpu/x86/16bit/entry16.inc
crt0-y += ../../../../src/cpu/x86/32bit/entry32.inc
crt0-y += ../../../../src/cpu/x86/16bit/reset16.inc
crt0-y += ../../../../src/southbridge/nvidia/mcp55/id.inc
crt0-y += ../../../../src/cpu/amd/car/cache_as_ram.inc
crt0-y += auto.inc

ldscript-y += ../../../../src/arch/i386/init/ldscript_fallback_cbfs.lb
ldscript-y += ../../../../src/cpu/x86/16bit/entry16.lds
ldscript-y += ../../../../src/cpu/x86/16bit/reset16.lds
ldscript-y += ../../../../src/southbridge/nvidia/mcp55/id.lds
ldscript-y += ../../../../src/arch/i386/lib/failover.lds
ldscript-$(CONFIG_AP_CODE_IN_CAR) += ../../../../src/arch/i386/init/ldscript_apc.lb

ifdef POST_EVALUATION

$(obj)/dsdt.c: $(src)/mainboard/$(MAINBOARDDIR)/dsdt.asl
	iasl -p $(obj)/dsdt -tc $(src)/mainboard/$(MAINBOARDDIR)/dsdt.asl
	mv $(obj)/dsdt.hex $@

$(obj)/mainboard/$(MAINBOARDDIR)/dsdt.o: $(obj)/dsdt.c
	$(CC) $(DISTRO_CFLAGS) $(CFLAGS) $(CPPFLAGS) $(DEBUG_CFLAGS) -I$(src) -I. -c $< -o $@

$(obj)/mainboard/$(MAINBOARDDIR)/apc_auto.o: $(src)/mainboard/$(MAINBOARDDIR)/apc_auto.c $(obj)/option_table.h
	$(CC) $(DISTRO_CFLAGS) $(CFLAGS) $(CPPFLAGS) $(DEBUG_CFLAGS) -I$(src) -I. -c -S  $(src)/mainboard/$(MAINBOARDDIR)/apc_auto.c -o $@

$(obj)/mainboard/$(MAINBOARDDIR)/auto.inc: $(src)/mainboard/$(MAINBOARDDIR)/cache_as_ram_auto.c $(obj)/option_table.h
	$(CC) $(DISTRO_CFLAGS) $(CFLAGS) $(CPPFLAGS) $(DEBUG_CFLAGS) -I$(src) -I. -c -S  $(src)/mainboard/$(MAINBOARDDIR)/cache_as_ram_auto.c -o $@
	perl -e 's/\.rodata/.rom.data/g' -pi $@
	perl -e 's/\.text/.section .rom.text/g' -pi $@

endif

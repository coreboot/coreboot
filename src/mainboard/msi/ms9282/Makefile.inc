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
obj-$(CONFIG_GENERATE_MP_TABLE) += mptable.o
obj-$(CONFIG_GENERATE_PIRQ_TABLE) += irq_tables.o
obj-$(CONFIG_USE_INIT) += romstage.o
obj-$(CONFIG_AP_CODE_IN_CAR) += ap_romstage.o

# This is part of the conversion to init-obj and away from included code. 
initobj-y += crt0.o
crt0s := $(src)/cpu/x86/16bit/entry16.inc
crt0s += $(src)/cpu/x86/32bit/entry32.inc
crt0s += $(src)/cpu/x86/16bit/reset16.inc
crt0s += $(src)/arch/i386/lib/id.inc
crt0s += $(src)/cpu/amd/car/cache_as_ram.inc
crt0s += $(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc

ldscripts := $(src)/arch/i386/init/ldscript_fallback_cbfs.lb
ldscripts += $(src)/cpu/x86/16bit/entry16.lds
ldscripts += $(src)/cpu/x86/16bit/reset16.lds
ldscripts += $(src)/arch/i386/lib/id.lds
ldscripts += $(src)/arch/i386/lib/failover.lds
ifeq ($(CONFIG_AP_CODE_IN_CAR),y)
ldscripts += $(src)/arch/i386/init/ldscript_apc.lb
endif

ifdef POST_EVALUATION

$(obj)/mainboard/$(MAINBOARDDIR)/ap_romstage.o: $(src)/mainboard/$(MAINBOARDDIR)/ap_romstage.c $(obj)/option_table.h
	$(CC) $(DISTRO_CFLAGS) $(CFLAGS) $(CPPFLAGS) $(DEBUG_CFLAGS) -I$(src) -I. -c -S  $(src)/mainboard/$(MAINBOARDDIR)/ap_romstage.c -o $@

endif

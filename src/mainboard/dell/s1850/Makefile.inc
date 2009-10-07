##
## This file is part of the coreboot project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
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

initobj-y += crt0.o
crt0-y += ../../../../src/cpu/x86/16bit/entry16.inc
crt0-y += ../../../../src/cpu/x86/32bit/entry32.inc
crt0-y += ../../../../src/cpu/x86/16bit/reset16.inc
crt0-y += ../../../../src/arch/i386/lib/cpu_reset.inc
crt0-y += ../../../../src/arch/i386/lib/id.inc
crt0-y += ../../../../src/cpu/x86/fpu/enable_fpu.inc
crt0-y += ../../../../src/cpu/x86/mmx/enable_mmx.inc
crt0-y += ../../../../src/cpu/x86/sse/enable_sse.inc
crt0-y += failover.inc
crt0-y += auto.inc
crt0-y += ../../../../src/cpu/x86/sse/disable_sse.inc
crt0-y += ../../../../src/cpu/x86/mmx/disable_mmx.inc

ldscript-y += ../../../../src/arch/i386/init/ldscript_fallback_cbfs.lb
ldscript-y += ../../../../src/cpu/x86/16bit/entry16.lds
ldscript-y += ../../../../src/cpu/x86/32bit/entry32.lds
ldscript-y += ../../../../src/cpu/x86/16bit/reset16.lds
ldscript-y += ../../../../src/arch/i386/lib/id.lds
ldscript-y += ../../../../src/arch/i386/lib/failover.lds

driver-y += mainboard.o

obj-$(CONFIG_HAVE_MP_TABLE) += mptable.o
obj-$(CONFIG_HAVE_PIRQ_TABLE) += irq_tables.o

ifdef POST_EVALUATION

ROMCCFLAGS ?= -mcpu=p4 -O2

$(obj)/mainboard/$(MAINBOARDDIR)/failover.inc: $(obj)/romcc $(src)/arch/i386/lib/failover.c
	$(obj)/romcc $(ROMCCFLAGS) --label-prefix=failover $(INCLUDES) $(src)/arch/i386/lib/failover.c -o $@

ifeq ($(CONFIG_HAVE_OPTION_TABLE),y)
$(obj)/mainboard/$(MAINBOARDDIR)/auto.inc: $(obj)/romcc $(src)/mainboard/$(MAINBOARDDIR)/auto.c $(obj)/option_table.h
	$(obj)/romcc $(ROMCCFLAGS) $(INCLUDES) $(src)/mainboard/$(MAINBOARDDIR)/auto.c -o $@
else
$(obj)/mainboard/$(MAINBOARDDIR)/auto.inc: $(obj)/romcc $(src)/mainboard/$(MAINBOARDDIR)/auto.c
	$(obj)/romcc $(ROMCCFLAGS) $(INCLUDES) $(src)/mainboard/$(MAINBOARDDIR)/auto.c -o $@
endif

endif


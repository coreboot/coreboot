##
## This file is part of the coreboot project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
## Copyright (C) 2010 coresystems GmbH
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

crt0s :=
ifeq($(CONFIG_BIG_BOOTBLOCK),y)
crt0s += $(src)/cpu/x86/16bit/entry16.inc
endif
crt0s += $(src)/cpu/x86/32bit/entry32.inc
ifeq($(CONFIG_BIG_BOOTBLOCK),y)
crt0s += $(src)/cpu/x86/16bit/reset16.inc
crt0s += $(src)/arch/i386/lib/cpu_reset.inc
crt0s += $(src)/arch/i386/lib/id.inc
endif
crt0s += $(src)/cpu/x86/32bit/entry32.inc
crt0s += $(src)/cpu/x86/fpu_enable.inc

ifeq($(CONFIG_CPU_AMD_GX1),y)
crt0s += $(src)/cpu/amd/model_gx1/cpu_setup.inc
crt0s += $(src)/cpu/amd/model_gx1/gx_setup.inc
endif
ifeq($(CONFIG_SSE),y)
crt0s += $(src)/cpu/x86/sse_enable.inc
endif
ifeq($(CONFIG_BIG_BOOTBLOCK),y)
crt0s += $(obj)/mainboard/$(MAINBOARDDIR)/failover.inc
endif
crt0s += $(obj)/mainboard/$(MAINBOARDDIR)/auto.inc
ifeq($(CONFIG_SSE),y)
crt0s += $(src)/cpu/x86/sse_disable.inc
endif
ifeq($(CONFIG_MMX),y)
crt0s += $(src)/cpu/x86/mmx_disable.inc
endif

ldscripts := $(src)/arch/i386/init/ldscript_fallback_cbfs.lb
ifeq($(CONFIG_BIG_BOOTBLOCK),y)
ldscripts += $(src)/cpu/x86/16bit/entry16.lds
endif
ldscripts += $(src)/cpu/x86/32bit/entry32.lds
ifeq($(CONFIG_BIG_BOOTBLOCK),y)
ldscripts += $(src)/cpu/x86/16bit/reset16.lds
ldscripts += $(src)/arch/i386/lib/id.lds
ldscripts += $(src)/arch/i386/lib/failover.lds
endif

driver-y += mainboard.o

obj-$(CONFIG_GENERATE_MP_TABLE) += mptable.o
obj-$(CONFIG_GENERATE_PIRQ_TABLE) += irq_tables.o
obj-$(CONFIG_HAVE_HARD_RESET) += reset.o

ifdef POST_EVALUATION

ROMCCFLAGS ?= -mcpu=p2 -O2

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


##
## This file is part of the coreboot project.
##
## Copyright (C) 2008-2010 coresystems GmbH
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
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smmrelocate.S
ifeq ($(CONFIG_HAVE_SMI_HANDLER),y)
ramstage-srcs += $(obj)/cpu/x86/smm/smm_wrap
endif

smm-y += smmhandler.S
smm-y += smihandler.c
smm-y += smiutil.c

$(obj)/cpu/x86/smm/smm.o: $$(smm-objs)
	$(CC) $(LDFLAGS) -nostdlib -r -o $@ $^

$(obj)/cpu/x86/smm/smm_wrap: $(obj)/cpu/x86/smm/smm.o $(src)/cpu/x86/smm/smm.ld $(obj)/ldoptions
	$(CC) $(LDFLAGS) -nostdlib -nostartfiles -static -o $(obj)/cpu/x86/smm/smm.elf -T $(src)/cpu/x86/smm/smm.ld $(obj)/cpu/x86/smm/smm.o
	$(NM) -n $(obj)/cpu/x86/smm/smm.elf | sort > $(obj)/cpu/x86/smm/smm.map
	$(OBJCOPY) -O binary $(obj)/cpu/x86/smm/smm.elf $(obj)/cpu/x86/smm/smm

# change to the target path because objcopy will use the path name in its
# ELF symbol names.
$(obj)/cpu/x86/smm/smm_wrap.ramstage.o: $(obj)/cpu/x86/smm/smm_wrap
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	cd $(obj)/cpu/x86/smm; $(OBJCOPY) -I binary smm -O elf32-i386 -B i386 smm_wrap.ramstage.o


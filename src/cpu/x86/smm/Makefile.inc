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

obj-$(CONFIG_HAVE_SMI_HANDLER) += smmrelocate.o
obj-$(CONFIG_HAVE_SMI_HANDLER) += smm_wrap.o

smmobj-y += smmhandler.o
smmobj-y += smihandler.o
smmobj-y += smiutil.o

$(obj)/cpu/x86/smm/smm.o: $$(smmobjs)
	$(CC) $(LDFLAGS) -nostdlib -r -o $@ $^

$(obj)/cpu/x86/smm/smm: $(obj)/cpu/x86/smm/smm.o $(src)/cpu/x86/smm/smm.ld $(obj)/ldoptions
	$(CC) $(LDFLAGS) -nostdlib -nostartfiles -static -o $(obj)/cpu/x86/smm/smm.elf -T $(src)/cpu/x86/smm/smm.ld $(obj)/cpu/x86/smm/smm.o
	$(NM) -n $(obj)/cpu/x86/smm/smm.elf | sort > $(obj)/cpu/x86/smm/smm.map
	$(OBJCOPY) -O binary $(obj)/cpu/x86/smm/smm.elf $(obj)/cpu/x86/smm/smm

# change to the target path because objcopy will use the path name in its
# ELF symbol names.
$(obj)/cpu/x86/smm/smm_wrap.o: $(obj)/cpu/x86/smm/smm
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	cd $(obj)/cpu/x86/smm; $(OBJCOPY) -I binary smm -O elf32-i386 -B i386 smm_wrap.o


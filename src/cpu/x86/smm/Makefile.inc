##
## This file is part of the coreboot project.
##
## Copyright (C) 2008 coresystems GmbH
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



obj-$(CONFIG_HAVE_SMI_HANDLER) += smmrelocate.o
obj-$(CONFIG_HAVE_SMI_HANDLER) += smm_bin.o

smmobj-y += smmhandler.o
smmobj-y += smihandler.o
smmobj-y += smiutil.o

ifdef POST_EVALUATION

$(obj)/cpu/x86/smm/smm.o: $(smmobjs) $(obj)/console/printk.o $(obj)/console/vtxprintf.o $(LIBGCC_FILE_NAME)
	$(CC) $(LDFLAGS) -nostdlib -r -o $@ $^

$(obj)/cpu/x86/smm/smm: $(obj)/cpu/x86/smm/smm.o $(src)/cpu/x86/smm/smm.ld $(obj)/ldoptions
	$(CC) $(LDFLAGS) -nostdlib -nostartfiles -static -o smm.elf -T $(src)/cpu/x86/smm/smm.ld $(obj)/cpu/x86/smm/smm.o
	$(CONFIG_CROSS_COMPILE)nm -n smm.elf | sort > smm.map
	$(OBJCOPY) -O binary smm.elf smm

$(obj)/cpu/x86/smm/smm_bin.c: $(obj)/cpu/x86/smm/smm
	(echo 'unsigned char smm[] = {'; od -vtx1 smm | sed -e 's,^[0-9]* *,,' -e 's:[0-9a-f][0-9a-f] :0x&,:g' -e 's:[0-9a-f][0-9a-f]$$:0x&,:'; echo '}; unsigned int smm_len = '; wc -c smm |awk '{print $$1;}' ; echo ';')  > $@

endif


#
# This file is part of the coreboot project.
#
# Copyright (C) 2010 Google Inc.
# Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#
subdirs-y += microcode
subdirs-y += romstage
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-$(CONFIG_HAVE_SMI_HANDLER) += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/x86/cache
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../lib/fsp
subdirs-y += fsp

ramstage-y += memmap.c
romstage-y += memmap.c
ramstage-y += tsc_freq.c
romstage-y += tsc_freq.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += tsc_freq.c
ramstage-$(CONFIG_CACHE_MRC_SETTINGS) += nvm.c
ramstage-y += spi.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += spi.c
ramstage-y += chip.c
ramstage-y += iosf.c
romstage-y += iosf.c
ramstage-y += northcluster.c
ramstage-y += ramstage.c
ramstage-y += gpio.c
romstage-y += gpio.c
ramstage-y += pmutil.c
ramstage-y += southcluster.c
romstage-y += reset.c
ramstage-y += reset.c
ramstage-y += cpu.c
ramstage-y += acpi.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += pmutil.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smm.c

ramstage-y += placeholders.c
ramstage-y += i2c.c

CPPFLAGS_common += -I$(src)/soc/intel/fsp_baytrail/
CPPFLAGS_common += -I$(src)/soc/intel/fsp_baytrail/fsp

# Run an intermediate step when producing coreboot.rom
# that adds additional components to the final firmware
# image outside of CBFS
ifeq ($(CONFIG_INCLUDE_ME),y)
ifneq ($(CONFIG_ME_PATH),)
INTERMEDIATE:=baytrail_add_txe

baytrail_add_txe: $(obj)/coreboot.pre $(IFDTOOL)
	printf "    DD         Adding Intel Firmware Descriptor\n"
	dd if=$(call strip_quotes,$(CONFIG_ME_PATH))/descriptor.bin \
		of=$(obj)/coreboot.pre conv=notrunc >/dev/null 2>&1
	printf "    IFDTOOL    txe.bin -> coreboot.pre\n"
	$(objutil)/ifdtool/ifdtool \
		-i ME:$(call strip_quotes,$(CONFIG_ME_PATH))/txe.bin \
		$(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
ifeq ($(CONFIG_LOCK_MANAGEMENT_ENGINE),y)
	printf "    IFDTOOL    Locking Management Engine\n"
	$(objutil)/ifdtool/ifdtool -l $(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
else
	printf "    IFDTOOL    Unlocking Management Engine\n"
	$(objutil)/ifdtool/ifdtool -u $(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
endif

endif
endif

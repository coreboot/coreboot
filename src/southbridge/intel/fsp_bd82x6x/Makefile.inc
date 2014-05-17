##
## This file is part of the coreboot project.
##
## Copyright (C) 2010 Google Inc.
## Copyright (C) 2013 Sage Electronic Engineering, LLC.
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

# Run an intermediate step when producing coreboot.rom
# that adds additional components to the final firmware
# image outside of CBFS
ifeq ($(CONFIG_INCLUDE_ME),y)
INTERMEDIATE+=bd82x6x_add_me
endif

ramstage-y += pch.c
ramstage-y += azalia.c
ramstage-y += lpc.c
ramstage-y += sata.c
ramstage-y += me.c
ramstage-y += me_8.x.c
ramstage-y += me_status.c
ramstage-y += reset.c
ramstage-y += watchdog.c

ramstage-$(CONFIG_ELOG) += elog.c
ramstage-y += ../common/spi.c
smm-$(CONFIG_SPI_FLASH_SMM) += ../common/spi.c

ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c me.c me_8.x.c finalize.c

romstage-y += early_usb.c early_smbus.c early_me.c me_status.c gpio.c early_init.c
romstage-$(CONFIG_USBDEBUG) += usb_debug.c
ramstage-$(CONFIG_USBDEBUG) += usb_debug.c
smm-$(CONFIG_USBDEBUG) += usb_debug.c
romstage-y += reset.c
romstage-y += early_spi.c

bd82x6x_add_me: $(obj)/coreboot.pre $(IFDTOOL)
	printf "    DD         Adding Intel Firmware Descriptor\n"
	dd if=$(call strip_quotes,$(CONFIG_ME_PATH))/descriptor.bin \
		of=$(obj)/coreboot.pre conv=notrunc >/dev/null 2>&1
	printf "    IFDTOOL    me.bin -> coreboot.pre\n"
	$(objutil)/ifdtool/ifdtool \
		-i ME:$(call strip_quotes,$(CONFIG_ME_PATH))/me.bin \
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

PHONY += bd82x6x_add_me

CPPFLAGS_common += -I$(src)/southbridge/intel/fsp_bd82x6x

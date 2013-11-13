##
## This file is part of the coreboot project.
##
## Copyright (C) 2010 Google Inc.
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
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

# Run an intermediate step when producing coreboot.rom
# that adds additional components to the final firmware
# image outside of CBFS
INTERMEDIATE+=bd82x6x_add_me

ramstage-y += ../bd82x6x/pch.c
ramstage-y += azalia.c
ramstage-y += lpc.c
ramstage-y += ../bd82x6x/pci.c
ramstage-y += ../bd82x6x/pcie.c
ramstage-y += sata.c
ramstage-y += usb_ehci.c
ramstage-y += me.c
ramstage-y += ../bd82x6x/me_8.x.c
ramstage-y += smbus.c
ramstage-y += thermal.c

ramstage-y += ../bd82x6x/me_status.c
ramstage-y += ../bd82x6x/reset.c
ramstage-y += ../bd82x6x/watchdog.c

ramstage-$(CONFIG_ELOG) += ../bd82x6x/elog.c
ramstage-y += spi.c
smm-$(CONFIG_SPI_FLASH_SMM) += spi.c

ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c me.c ../bd82x6x/me_8.x.c ../bd82x6x/finalize.c ../bd82x6x/pch.c

romstage-y += ../bd82x6x/early_usb.c early_smbus.c ../bd82x6x/early_me.c ../bd82x6x/me_status.c ../bd82x6x/gpio.c
romstage-y += ../bd82x6x/reset.c
romstage-$(CONFIG_SOUTHBRIDGE_INTEL_BD82X6X) += ../bd82x6x/early_spi.c
romstage-$(CONFIG_SOUTHBRIDGE_INTEL_C216) += ../bd82x6x/early_spi.c

bd82x6x_add_me: $(obj)/coreboot.pre $(IFDTOOL)
	printf "    DD         Adding Intel Firmware Descriptor\n"
	dd if=3rdparty/mainboard/$(MAINBOARDDIR)/descriptor.bin \
		of=$(obj)/coreboot.pre conv=notrunc >/dev/null 2>&1
	printf "    IFDTOOL    me.bin -> coreboot.pre\n"
	$(objutil)/ifdtool/ifdtool \
		-i ME:3rdparty/mainboard/$(MAINBOARDDIR)/me.bin \
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

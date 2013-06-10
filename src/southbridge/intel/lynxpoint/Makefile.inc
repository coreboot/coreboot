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
# FIXME, uncomment as soon as we have ME firmware in the blobs repo
# INTERMEDIATE:=lynxpoint_add_me

ramstage-y += pch.c
ramstage-y += azalia.c
ramstage-y += lpc.c
ramstage-y += pci.c
ramstage-y += pcie.c
ramstage-y += sata.c
ramstage-y += usb_ehci.c
ramstage-y += me_9.x.c
ramstage-y += smbus.c
ramstage-$(CONFIG_INTEL_LYNXPOINT_LP) += serialio.c

ramstage-y += rcba.c
ramstage-y += me_status.c
ramstage-y += reset.c
ramstage-y += watchdog.c
ramstage-y += acpi.c
ramstage-$(CONFIG_ALT_CBFS_LOAD_PAYLOAD) += spi_loading.c

ramstage-$(CONFIG_ELOG) += elog.c
ramstage-y += spi.c
smm-$(CONFIG_SPI_FLASH_SMM) += spi.c

ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c pmutil.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c me_9.x.c finalize.c pch.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += pmutil.c

romstage-y += early_usb.c early_smbus.c early_me.c me_status.c early_pch.c
romstage-y += reset.c early_spi.c rcba.c

ifeq ($(CONFIG_INTEL_LYNXPOINT_LP),y)
romstage-y += lp_gpio.c
ramstage-y += lp_gpio.c
else
romstage-y += gpio.c
ramstage-y += gpio.c
endif

lynxpoint_add_me: $(obj)/coreboot.pre $(IFDTOOL)
	printf "    DD         Adding Intel Firmware Descriptor\n"
	dd if=$(CONFIG_IFD_BIN_PATH) \
		of=$(obj)/coreboot.pre conv=notrunc >/dev/null 2>&1
	printf "    IFDTOOL    me.bin -> coreboot.pre\n"
	$(objutil)/ifdtool/ifdtool \
		-i ME:$(CONFIG_ME_BIN_PATH) \
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

PHONY += lynxpoint_add_me

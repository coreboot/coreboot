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
ramstage-y += ../common/pciehp.c

ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/hda_verb.c

ramstage-y += ../bd82x6x/me_status.c
ramstage-y += ../bd82x6x/reset.c
ramstage-y += ../bd82x6x/watchdog.c

ramstage-$(CONFIG_ELOG) += ../bd82x6x/elog.c
ramstage-y += ../common/spi.c
ramstage-y += madt.c
smm-$(CONFIG_SPI_FLASH_SMM) += ../common/spi.c

ramstage-y += smi.c
smm-y += smihandler.c me.c ../bd82x6x/me_8.x.c ../bd82x6x/finalize.c ../bd82x6x/pch.c

romstage-y += ../bd82x6x/early_usb.c early_smbus.c ../bd82x6x/early_me.c ../bd82x6x/me_status.c ../bd82x6x/gpio.c early_thermal.c
romstage-y += ../bd82x6x/reset.c
romstage-y += ../bd82x6x/early_rcba.c
romstage-$(CONFIG_SOUTHBRIDGE_INTEL_BD82X6X) += ../bd82x6x/early_spi.c
romstage-$(CONFIG_SOUTHBRIDGE_INTEL_C216) += ../bd82x6x/early_spi.c

ifeq ($(CONFIG_BUILD_WITH_FAKE_IFD),y)
IFD_BIN_PATH := $(objgenerated)/ifdfake.bin
IFD_SECTIONS := $(addprefix -b ,$(CONFIG_IFD_BIOS_SECTION:"%"=%)) \
		$(addprefix -m ,$(CONFIG_IFD_ME_SECTION:"%"=%)) \
		$(addprefix -g ,$(CONFIG_IFD_GBE_SECTION:"%"=%)) \
		$(addprefix -p ,$(CONFIG_IFD_PLATFORM_SECTION:"%"=%))
else
IFD_BIN_PATH := $(CONFIG_IFD_BIN_PATH)
endif

bd82x6x_add_me: $(obj)/coreboot.pre $(IFDTOOL) $(IFDFAKE)
ifeq ($(CONFIG_BUILD_WITH_FAKE_IFD),y)
	printf "\n** WARNING **\n"
	printf "Coreboot will be built with a fake Intel Firmware Descriptor (IFD).\n"
	printf "Never write a complete coreboot.rom with a fake IFD to your board's\n"
	printf "flash ROM! Make sure that you only write valid flash regions.\n\n"
	printf "    IFDFAKE    Building a fake Intel Firmware Descriptor\n"
	$(IFDFAKE) $(IFD_SECTIONS) $(IFD_BIN_PATH)
endif
	printf "    DD         Adding Intel Firmware Descriptor\n"
	dd if=$(IFD_BIN_PATH) \
		of=$(obj)/coreboot.pre conv=notrunc >/dev/null 2>&1
ifeq ($(CONFIG_HAVE_ME_BIN),y)
	printf "    IFDTOOL    me.bin -> coreboot.pre\n"
	$(objutil)/ifdtool/ifdtool \
		-i ME:$(CONFIG_ME_BIN_PATH) \
		$(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
else
	printf "\n** WARNING **\n"
	printf "Coreboot will be built without Management Engine firmware.\n"
	printf "Never write a complete coreboot.rom without ME to your board's\n"
	printf "flash ROM! Make sure that you only write valid flash regions.\n\n"
endif
ifeq ($(CONFIG_LOCK_MANAGEMENT_ENGINE),y)
	printf "    IFDTOOL    Locking Management Engine\n"
	$(objutil)/ifdtool/ifdtool -l $(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
else ifneq ($(CONFIG_BUILD_WITH_FAKE_IFD),y)
	printf "    IFDTOOL    Unlocking Management Engine\n"
	$(objutil)/ifdtool/ifdtool -u $(obj)/coreboot.pre
	mv $(obj)/coreboot.pre.new $(obj)/coreboot.pre
endif

PHONY += bd82x6x_add_me

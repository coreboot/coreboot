################################################################################
##
## This file is part of the coreboot project.
##
## Copyright (C) 2009-2010 coresystems GmbH
## Copyright (C) 2015 Google Inc.
## Copyright (C) 2017 Facebook Inc.
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

######################################################################
# set up payload config and version files for later inclusion
ifeq ($(CONFIG_PAYLOAD_SEABIOS),y)
PAYLOAD_CONFIG=payloads/external/SeaBIOS/seabios/.config
PAYLOAD_VERSION=payloads/external/SeaBIOS/seabios/out/autoversion.h
endif
ifeq ($(CONFIG_PAYLOAD_FILO),y)
PAYLOAD_CONFIG=payloads/external/FILO/filo/.config
PAYLOAD_VERSION=payloads/external/FILO/filo/build/version.h
endif
ifeq ($(CONFIG_PAYLOAD_DEPTHCHARGE),y)
PAYLOAD_CONFIG=payloads/external/depthcharge/depthcharge/.config
$(PAYLOAD_CONFIG): payloads/external/depthcharge/depthcharge/build/depthcharge.elf
#TODO: Figure out version
endif

ifeq ($(CONFIG_PAYLOAD_LINUXBOOT),y)
  ifeq ($(CONFIG_ARCH_X86),y)
    ifneq ($(strip $(call strip_quotes,$(CONFIG_LINUXBOOT_KERNEL_COMMANDLINE))),)
      ADDITIONAL_PAYLOAD_CONFIG+=-C $(CONFIG_LINUXBOOT_KERNEL_COMMANDLINE)
    endif
    ifneq ($(strip $(call strip_quotes,$(CONFIG_PAYLOAD_USERSPACE))),)
      ADDITIONAL_PAYLOAD_CONFIG+=-I $(strip $(call strip_quotes,$(CONFIG_PAYLOAD_USERSPACE)))
    endif
  endif
endif

ifeq ($(CONFIG_PAYLOAD_LINUX),y)
ifneq ($(strip $(call strip_quotes,$(CONFIG_LINUX_COMMAND_LINE))),)
      ADDITIONAL_PAYLOAD_CONFIG+=-C $(CONFIG_LINUX_COMMAND_LINE)
endif
ifneq ($(strip $(call strip_quotes,$(CONFIG_LINUX_INITRD))),)
      ADDITIONAL_PAYLOAD_CONFIG+=-I $(CONFIG_LINUX_INITRD)
endif
endif
ifneq ($(strip $(call strip_quotes,$(CONFIG_PAYLOAD_OPTIONS))),)
      ADDITIONAL_PAYLOAD_CONFIG+=$(strip $(call strip_quotes,$(CONFIG_PAYLOAD_OPTIONS)))
endif

cbfs-files-y += $(CONFIG_CBFS_PREFIX)/payload
$(CONFIG_CBFS_PREFIX)/payload-file := $(CONFIG_PAYLOAD_FILE)
ifeq ($(CONFIG_PAYLOAD_IS_FLAT_BINARY),y)
$(CONFIG_CBFS_PREFIX)/payload-type := flat-binary
else
$(CONFIG_CBFS_PREFIX)/payload-type := payload
endif
$(CONFIG_CBFS_PREFIX)/payload-compression := $(CBFS_PAYLOAD_COMPRESS_FLAG)
$(CONFIG_CBFS_PREFIX)/payload-options := $(ADDITIONAL_PAYLOAD_CONFIG)

cbfs-files-$(CONFIG_INCLUDE_CONFIG_FILE) += payload_config
payload_config-file := $(PAYLOAD_CONFIG)
payload_config-type := raw

cbfs-files-$(CONFIG_INCLUDE_CONFIG_FILE) += payload_revision
payload_revision-file := $(PAYLOAD_VERSION)
payload_revision-type := raw

cbfs-files-$(CONFIG_GRUB2_INCLUDE_RUNTIME_CONFIG_FILE) += etc/grub.cfg
etc/grub.cfg-file := $(CONFIG_GRUB2_RUNTIME_CONFIG_FILE)
etc/grub.cfg-type := raw
etc/grub.cfg-required := the GRUB runtime configuration file ($(CONFIG_GRUB2_RUNTIME_CONFIG_FILE))

# SeaBIOS

SEABIOS_CC_OFFSET=$(if $(filter %ccache,$(HOSTCC)),2,1)
payloads/external/SeaBIOS/seabios/out/bios.bin.elf seabios: $(DOTCONFIG)
	$(MAKE) -C payloads/external/SeaBIOS \
			HOSTCC="$(HOSTCC)" \
			CC=$(word $(SEABIOS_CC_OFFSET),$(CC_x86_32)) \
			CFLAGS="$(patsubst $(word $(SEABIOS_CC_OFFSET),$(CC_x86_32))%,,$(wordlist $(SEABIOS_CC_OFFSET),9999,$(CC_x86_32)))" \
			LD=$(word 1,$(LD_x86_32)) LDFLAGS="$(patsubst $(word 1,$(LD_x86_32))%,,$(LD_x86_32))" \
			OBJDUMP="$(OBJDUMP_x86_32)" \
			OBJCOPY="$(OBJCOPY_x86_32)" STRIP="$(STRIP_x86_32)" \
			AS="$(AS_x86_32)" IASL="$(IASL)" \
			CONFIG_SEABIOS_MASTER=$(CONFIG_SEABIOS_MASTER) \
			CONFIG_SEABIOS_STABLE=$(CONFIG_SEABIOS_STABLE) \
			CONFIG_SEABIOS_REVISION=$(CONFIG_SEABIOS_REVISION) \
			CONFIG_SEABIOS_REVISION_ID=$(CONFIG_SEABIOS_REVISION_ID) \
			CONFIG_PAYLOAD_CONFIGFILE=$(CONFIG_PAYLOAD_CONFIGFILE) \
			CONFIG_SEABIOS_THREAD_OPTIONROMS=$(CONFIG_SEABIOS_THREAD_OPTIONROMS) \
			CONFIG_SEABIOS_VGA_COREBOOT=$(CONFIG_SEABIOS_VGA_COREBOOT) \
			CONFIG_DRIVERS_UART_8250IO=$(CONFIG_DRIVERS_UART_8250IO) \
			CONFIG_DRIVERS_UART_8250MEM=$(CONFIG_DRIVERS_UART_8250MEM) \
			CONFIG_HUDSON_UART=$(CONFIG_HUDSON_UART) \
			CONFIG_CONSOLE_SERIAL=$(CONFIG_CONSOLE_SERIAL) \
			CONFIG_TTYS0_BASE=$(CONFIG_TTYS0_BASE) \
			CONFIG_SEABIOS_DEBUG_LEVEL=$(CONFIG_SEABIOS_DEBUG_LEVEL) \
			CONFIG_DRIVERS_UART_8250MEM_32=$(CONFIG_DRIVERS_UART_8250MEM_32) \
			CONFIG_ENABLE_HSUART=$(CONFIG_ENABLE_HSUART) \
			CONFIG_CONSOLE_UART_BASE_ADDRESS=$(CONFIG_CONSOLE_UART_BASE_ADDRESS)

payloads/external/SeaBIOS/seabios/out/vgabios.bin: seabios
payloads/external/SeaBIOS/seabios/.config: payloads/external/SeaBIOS/seabios/out/bios.bin.elf
payloads/external/SeaBIOS/seabios/out/autoversion.h: payloads/external/SeaBIOS/seabios/out/bios.bin.elf

# add a SeaBIOS bootorder file
ifneq ($(strip $(CONFIG_SEABIOS_BOOTORDER_FILE)),)
cbfs-files-y += bootorder
bootorder-file := $(strip $(CONFIG_SEABIOS_BOOTORDER_FILE))
bootorder-type := raw
endif

# Depthcharge

payloads/external/depthcharge/depthcharge/build/depthcharge.elf depthcharge: $(DOTCONFIG) $(CBFSTOOL)
	$(MAKE) -C payloads/external/depthcharge \
		BOARD=$(call ws_to_under,$(call strip_quotes,$(call tolower,$(CONFIG_MAINBOARD_PART_NUMBER)))) \
		MFLAGS= MAKEFLAGS= \
		DEPTHCHARGE_MASTER=$(CONFIG_DEPTHCHARGE_MASTER) \
		DEPTHCHARGE_STABLE=$(CONFIG_DEPTHCHARGE_STABLE) \
		DEPTHCHARGE_REVISION=$(CONFIG_DEPTHCHARGE_REVISION) \
		DEPTHCHARGE_REVISION_ID=$(CONFIG_DEPTHCHARGE_REVISION_ID) \
		OVERRIDE_DEFCONFIG=$(CONFIG_LP_DEFCONFIG_OVERRIDE)

# Tianocore

payloads/external/tianocore/tianocore/Build/UEFIPAYLOAD.fd tianocore: $(DOTCONFIG)
	$(MAKE) -C payloads/external/tianocore all \
		HOSTCC="$(HOSTCC)" \
		CC="$(HOSTCC)" \
		CONFIG_TIANOCORE_MASTER=$(CONFIG_TIANOCORE_MASTER) \
		CONFIG_TIANOCORE_STABLE=$(CONFIG_TIANOCORE_STABLE) \
		CONFIG_TIANOCORE_REVISION=$(CONFIG_TIANOCORE_REVISION) \
		CONFIG_TIANOCORE_REVISION_ID=$(CONFIG_TIANOCORE_REVISION_ID) \
		CONFIG_TIANOCORE_DEBUG=$(CONFIG_TIANOCORE_DEBUG) \
		CONFIG_TIANOCORE_TARGET_IA32=$(CONFIG_TIANOCORE_TARGET_IA32) \
		GCC_CC_x86_32=$(GCC_CC_x86_32) \
		GCC_CC_x86_64=$(GCC_CC_x86_64) \
		GCC_CC_arm=$(GCC_CC_arm) \
		GCC_CC_arm64=$(GCC_CC_arm64) \
		OBJCOPY_x86_32=$(OBJCOPY_x86_32) \
		OBJCOPY_x86_64=$(OBJCOPY_x86_64) \
		OBJCOPY_arm=$(OBJCOPY_arm) \
		OBJCOPY_arm64=$(OBJCOPY_arm64) \
		MFLAGS= MAKEFLAGS=

# FILO

filo:
	$(MAKE) -C payloads/external/FILO \
			HOSTCC="$(HOSTCC)" \
			CC="$(CC_x86_32)" LD="$(LD_x86_32)" OBJDUMP="$(OBJDUMP_x86_32)" \
			OBJCOPY="$(OBJCOPY_x86_32)" STRIP="$(STRIP_x86_32)" \
			CONFIG_FILO_MASTER=$(CONFIG_FILO_MASTER) \
			CONFIG_FILO_STABLE=$(CONFIG_FILO_STABLE)

payloads/external/FILO/filo/build/filo.elf: filo
payloads/external/FILO/filo/.config: filo
payloads/external/FILO/filo/build/version.h: filo

# Grub

grub2: $(obj)/config.h
	$(MAKE) -C payloads/external/GRUB2 \
			HOSTCC="$(HOSTCC)" \
			CC="$(CC_x86_32)" LD="$(LD_x86_32)" \
			OBJCOPY="$(OBJCOPY_x86_32)" STRIP="$(STRIP_x86_32)" \
			CONFIG_DEP="$(abspath $(obj)/config.h)" \
			CONFIG_GRUB2_STABLE=$(CONFIG_GRUB2_STABLE) \
			CONFIG_GRUB2_MASTER=$(CONFIG_GRUB2_MASTER) \
			CONFIG_GRUB2_REVISION=$(CONFIG_GRUB2_REVISION) \
			CONFIG_GRUB2_REVISION_ID=$(CONFIG_GRUB2_REVISION_ID) \
			CONFIG_GRUB2_EXTRA_MODULES=$(CONFIG_GRUB2_EXTRA_MODULES)

payloads/external/GRUB2/grub2/build/default_payload.elf: grub2

# U-Boot

payloads/external/U-Boot/u-boot/u-boot-dtb.bin u-boot: $(DOTCONFIG)
	$(MAKE) -C payloads/external/U-Boot \
			CONFIG_UBOOT_MASTER=$(CONFIG_UBOOT_MASTER) \
			CONFIG_UBOOT_STABLE=$(CONFIG_UBOOT_STABLE)

# TINT

payloads/external/tint/tint/tint.elf tint:
	$(MAKE) -C payloads/external/tint

cbfs-files-$(CONFIG_TINT_SECONDARY_PAYLOAD) += img/tint
img/tint-file := payloads/external/tint/tint/tint.elf
img/tint-type := payload
img/tint-compression := $(CBFS_SECONDARY_PAYLOAD_COMPRESS_FLAG)

# Memtest86+

cbfs-files-$(CONFIG_MEMTEST_SECONDARY_PAYLOAD) += img/memtest
img/memtest-file := payloads/external/Memtest86Plus/memtest86plus/memtest
img/memtest-type := payload
img/memtest-compression := $(CBFS_SECONDARY_PAYLOAD_COMPRESS_FLAG)

ifeq ($(CONFIG_CONSOLE_SERIAL)$(CONFIG_DRIVERS_UART_8250IO),yy)
	MEMTEST_SERIAL_OPTIONS=SERIAL_CONSOLE_DEFAULT=1 \
		SERIAL_TTY=$(CONFIG_UART_FOR_CONSOLE) \
		SERIAL_BAUD_RATE=$(CONFIG_TTYS0_BAUD)
endif

payloads/external/Memtest86Plus/memtest86plus/memtest: $(DOTCONFIG)
	$(MAKE) -C payloads/external/Memtest86Plus all \
		CC="$(CC_x86_32)" \
		LD="$(LD_x86_32)" \
		OBJCOPY="$(OBJCOPY_x86_32)" \
		AS="$(AS_x86_32)" \
		CONFIG_MEMTEST_REVISION=$(CONFIG_MEMTEST_REVISION) \
		CONFIG_MEMTEST_REVISION_ID=$(CONFIG_MEMTEST_REVISION_ID) \
		CONFIG_MEMTEST_MASTER=$(CONFIG_MEMTEST_MASTER) \
		CONFIG_MEMTEST_STABLE=$(CONFIG_MEMTEST_STABLE) \
		$(MEMTEST_SERIAL_OPTIONS) \
		MFLAGS= MAKEFLAGS=

# iPXE

PXE_ROM_PCI_ID:=$(subst $(comma),,$(CONFIG_PXE_ROM_ID))

ifeq ($(CONFIG_PXE_ROM),y)
PXE_ROM_FILE:=$(CONFIG_PXE_ROM_FILE)
endif
ifeq ($(CONFIG_BUILD_IPXE),y)
PXE_ROM_FILE:=payloads/external/iPXE/ipxe/ipxe.rom
endif

ifeq ($(CONFIG_CONSOLE_SERIAL)$(CONFIG_DRIVERS_UART_8250IO),yy)
IPXE_UART=COM$(call int-add,$(CONFIG_UART_FOR_CONSOLE) 1)
endif

ifeq ($(CONFIG_PXE_SERIAL_CONSOLE),y)
IPXE_SERIAL_CONSOLE = $(CONFIG_CONSOLE_SERIAL)$(CONFIG_DRIVERS_UART_8250IO)
else
IPXE_SERIAL_CONSOLE = n
endif

cbfs-files-$(CONFIG_PXE_ROM)$(CONFIG_BUILD_IPXE) += pci$(CONFIG_PXE_ROM_ID).rom
pci$(CONFIG_PXE_ROM_ID).rom-file := $(PXE_ROM_FILE)
pci$(CONFIG_PXE_ROM_ID).rom-type := raw

payloads/external/iPXE/ipxe/ipxe.rom ipxe: $(DOTCONFIG)
	$(MAKE) -C payloads/external/iPXE all \
	CROSS_COMPILE="$(CROSS_COMPILE_$(ARCH-ramstage-y))" \
	PXE_ROM_PCI_ID=$(PXE_ROM_PCI_ID) \
	CONFIG_IPXE_MASTER=$(CONFIG_IPXE_MASTER) \
	CONFIG_IPXE_STABLE=$(CONFIG_IPXE_STABLE) \
	CONSOLE_SERIAL=$(IPXE_SERIAL_CONSOLE) \
	IPXE_UART=$(IPXE_UART) \
	CONFIG_TTYS0_BAUD=$(CONFIG_TTYS0_BAUD) \
	MFLAGS= MAKEFLAGS=

# LinuxBoot

linuxboot:
	$(MAKE) -C payloads/external/LinuxBoot \
		CPUS=$(CPUS) \
		CONFIG_LINUXBOOT_KERNEL_VERSION=$(CONFIG_LINUXBOOT_KERNEL_VERSION) \
		CONFIG_LINUXBOOT_KERNEL_CONFIGFILE=$(CONFIG_LINUXBOOT_KERNEL_CONFIGFILE) \
		CONFIG_LINUXBOOT_KERNEL_COMMANDLINE=$(CONFIG_LINUXBOOT_KERNEL_COMMANDLINE) \
		CONFIG_LINUXBOOT_UROOT_VERSION=$(CONFIG_LINUXBOOT_UROOT_VERSION) \
		CONFIG_LINUXBOOT_UROOT_COMMANDS=$(CONFIG_LINUXBOOT_UROOT_COMMANDS) \
		CONFIG_LINUXBOOT_ARCH=$(CONFIG_LINUXBOOT_ARCH) \
		CONFIG_LINUXBOOT_UROOT=$(CONFIG_LINUXBOOT_UROOT) \
		CONFIG_LINUXBOOT_UROOT_FILES=$(CONFIG_LINUXBOOT_UROOT_FILES) \
		CONFIG_LINUXBOOT_DTB_FILE=$(CONFIG_LINUXBOOT_DTB_FILE)


payloads/external/LinuxBoot/linuxboot/kernel-image: linuxboot
payloads/external/LinuxBoot/linuxboot/initramfs.cpio.xz: linuxboot
payloads/external/LinuxBoot/linuxboot/kernel/.config: linuxboot
payloads/external/LinuxBoot/linuxboot/go/src/github.com/u-root/u-root/.git: linuxboot

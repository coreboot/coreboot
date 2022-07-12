## SPDX-License-Identifier: GPL-2.0-only

# force the shell to bash - the edksetup.sh script doesn't work with dash
export SHELL := env bash

project_name = Tianocore
project_dir = $(CURDIR)/$(word 3,$(subst /, ,$(CONFIG_TIANOCORE_REPOSITORY)))

BUILD_STR = -a IA32 -a X64 -t COREBOOT
ifeq ($(CONFIG_TIANOCORE_COREBOOTPAYLOAD),y)
BUILD_STR += -p CorebootPayloadPkg/CorebootPayloadPkgIa32X64.dsc
else
BUILD_STR += -p UefiPayloadPkg/UefiPayloadPkg.dsc
endif
BUILD_STR += -D BOOTLOADER=COREBOOT -q

#
# EDK II has the following build options relevant to coreboot:
#
#
# OPTION			= DEFAULT_VALUE
#
# ABOVE_4G_MEMORY		= TRUE
ifneq ($(CONFIG_TIANOCORE_ABOVE_4G_MEMORY),y)
BUILD_STR += -D ABOVE_4G_MEMORY=FALSE
endif
# BOOTSPLASH_IMAGE		= FALSE
ifneq ($(CONFIG_TIANOCORE_BOOTSPLASH_FILE),)
BUILD_STR += -D BOOTSPLASH_IMAGE=TRUE
endif
# BOOT_MANAGER_ESCAPE		= FALSE
ifeq ($(CONFIG_TIANOCORE_BOOT_MANAGER_ESCAPE),y)
BUILD_STR += -D BOOT_MANAGER_ESCAPE=TRUE
endif
# BUILD_TARGETS			= DEBUG
ifeq ($(CONFIG_TIANOCORE_RELEASE),y)
BUILD_STR += -b RELEASE
endif
# DISABLE_SERIAL_TERMINAL	= FALSE
ifneq ($(CONFIG_TIANOCORE_SERIAL_SUPPORT),y)
BUILD_STR += -D DISABLE_SERIAL_TERMINAL=TRUE
endif
# FOLLOW_BGRT_SPEC		= FALSE
ifeq ($(CONFIG_TIANOCORE_FOLLOW_BGRT_SPEC),y)
BUILD_STR += -D FOLLOW_BGRT_SPEC=TRUE
endif
# PCIE_BASE_ADDRESS		= 0
ifneq ($(CONFIG_ECAM_MMCONF_LENGTH),)
BUILD_STR += --pcd gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress=$(CONFIG_ECAM_MMCONF_BASE_ADDRESS)
endif
# PCIE_BASE_LENGTH		= 0
ifneq ($(CONFIG_ECAM_MMCONF_LENGTH),)
BUILD_STR += --pcd gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseSize=$(CONFIG_ECAM_MMCONF_LENGTH)
endif
# PRIORITIZE_INTERNAL		= FALSE
ifeq ($(CONFIG_TIANOCORE_PRIORITIZE_INTERNAL),y)
BUILD_STR += -D PRIORITIZE_INTERNAL=TRUE
endif
# PS2_KEYBOARD_ENABLE		= FALSE
ifeq ($(CONFIG_TIANOCORE_PS2_SUPPORT),y)
BUILD_STR += -D PS2_KEYBOARD_ENABLE=TRUE
endif
# PLATFORM_BOOT_TIMEOUT		= 3
ifneq ($(CONFIG_TIANOCORE_BOOT_TIMEOUT),)
BUILD_STR += -D PLATFORM_BOOT_TIMEOUT=$(CONFIG_TIANOCORE_BOOT_TIMEOUT)
endif
# SIO_BUS_ENABLE		= FALSE
ifeq ($(CONFIG_TIANOCORE_PS2_SUPPORT),y)
BUILD_STR += -D SIO_BUS_ENABLE=TRUE
endif
# SHELL_TYPE			= BUILD_SHELL
ifneq ($(CONFIG_TIANOCORE_HAVE_EFI_SHELL),y)
BUILD_STR += -D SHELL_TYPE=NONE
endif
# USE_CBMEM_FOR_CONSOLE		= FALSE
ifeq ($(CONFIG_TIANOCORE_CBMEM_LOGGING),y)
BUILD_STR += -D USE_CBMEM_FOR_CONSOLE=TRUE
endif
# SD_MMC_TIMEOUT		= 1000000
ifneq ($(CONFIG_TIANOCORE_SD_MMC_TIMEOUT),)
BUILD_STR += -D SD_MMC_TIMEOUT=$(CONFIG_TIANOCORE_SD_MMC_TIMEOUT)
endif
#
# EDKII has the below PCDs that are revalant to coreboot:
#
# Allows EDKII to use the full framebuffer
BUILD_STR += --pcd gEfiMdeModulePkgTokenSpaceGuid.PcdConOutRow=0
BUILD_STR += --pcd gEfiMdeModulePkgTokenSpaceGuid.PcdConOutColumn=0
BUILD_STR += --pcd gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutRow=0
BUILD_STR += --pcd gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutColumn=0
#
# The below are legacy options only available in CorebootPayloadPkg:
#
# PCIE_BASE			= 0
ifeq ($(CONFIG_TIANOCORE_COREBOOTPAYLOAD),y)
ifneq ($(CONFIG_ECAM_MMCONF_BASE_ADDRESS),)
BUILD_STR += -D PCIE_BASE=$(CONFIG_ECAM_MMCONF_BASE_ADDRESS)
endif
# USE_HPET_TIMER		= FALSE
ifeq ($(CONFIG_TIANOCORE_USE_8254_TIMER),y)
BUILD_STR += -D USE_HPET_TIMER=TRUE
endif
endif # CONFIG_TIANOCORE_COREBOOTPAYLOAD

bootloader = $(word 8,$(subst /, ,$(BUILD_STR)))

ifeq ($(CONFIG_TIANOCORE_CUSTOM),y)
ifneq ($(CONFIG_TIANOCORE_CUSTOM_BUILD_PARAMS),)
BUILD_STR += $(CONFIG_TIANOCORE_CUSTOM_BUILD_PARAMS)
endif
endif

all: clean build

$(project_dir):
	echo "    Cloning $(project_name) from $(CONFIG_TIANOCORE_REPOSITORY)"
	git clone $(CONFIG_TIANOCORE_REPOSITORY) $(project_dir); \
		cd $(project_dir);

update: $(project_dir)
	if [ ! -d "$(project_dir)" ]; then \
		git clone $(CONFIG_TIANOCORE_REPOSITORY) $(project_dir); \
	fi
	cd  $(project_dir); \
		echo "    Fetching new commits from $(CONFIG_TIANOCORE_REPOSITORY)"; \
		git fetch origin 2>/dev/null; \
		if ! git rev-parse --verify -q $(CONFIG_TIANOCORE_TAG_OR_REV) >/dev/null; then \
			echo "    $(CONFIG_TIANOCORE_TAG_OR_REV) is not a valid git reference"; \
			exit 1; \
		fi; \
		if git status --ignore-submodules=dirty | grep -q clean; then \
			echo "    Checking out $(project_name) revision $(CONFIG_TIANOCORE_TAG_OR_REV)"; \
			git checkout --detach $(CONFIG_TIANOCORE_TAG_OR_REV) -f; \
		else \
			echo "    Working directory not clean; will not overwrite"; \
		fi; \
		git submodule update --init --checkout

checktools:
	echo "Checking uuid-dev..."
	echo "#include <uuid/uuid.h>" > libtest.c
	echo "int main(int argc, char **argv) { (void) argc; (void) argv; return 0; }" >> libtest.c
	$(HOSTCC) $(HOSTCCFLAGS) libtest.c -o libtest >/dev/null 2>&1 && echo " found uuid-dev." || \
		( echo " Not found."; echo "ERROR: please_install uuid-dev (libuuid-devel)"; exit 1 )
	rm -rf libtest.c libtest
	echo "Checking nasm..."
	type nasm > /dev/null 2>&1 && echo " found nasm." || \
		( echo " Not found."; echo "Error: Please install nasm."; exit 1 )

build: update checktools
	echo "   ##### $(project_name) Build Summary #####"
	echo "   Repository:     $(CONFIG_TIANOCORE_REPOSITORY)"
	echo "   Branch:         $(CONFIG_TIANOCORE_TAG_OR_REV)"
	echo " $(BUILD_STR)" | \
		sed 's/-/\n   /g' | sort | sed \
		-e 's/a /Architecture:   /g' \
		-e 's/b /Release:        /g' \
		-e 's/D /Option:         /g' \
		-e 's/p /Payload:        /g' \
		-e 's/q /Build:          Quiet/' \
		-e 's/t /Toolchain:      /'
	unset CC; $(MAKE) -C $(project_dir)/BaseTools 2>&1
	if [ -n "$(CONFIG_TIANOCORE_BOOTSPLASH_FILE)" ]; then \
		echo "    Copying custom bootsplash image"; \
		case "$(CONFIG_TIANOCORE_BOOTSPLASH_FILE)" in \
			/*) convert $(CONFIG_TIANOCORE_BOOTSPLASH_FILE) \
				BMP3:$(project_dir)/MdeModulePkg/Logo/Logo.bmp;; \
			*) convert $(top)/$(CONFIG_TIANOCORE_BOOTSPLASH_FILE) \
				BMP3:$(project_dir)/MdeModulePkg/Logo/Logo.bmp;; \
		esac \
	fi; \
	cd $(project_dir); \
		export EDK_TOOLS_PATH=$(project_dir)/BaseTools; \
		export WORKSPACE=$(project_dir); \
		. ./edksetup.sh BaseTools; \
		grep -q "COREBOOT" $(project_dir)/Conf/tools_def.txt; \
		if [ $$? -ne 0 ]; then \
			cat ../tools_def.txt >> $(project_dir)/Conf/tools_def.txt; \
		fi; \
		build $(BUILD_STR); \
		mkdir -p $(project_dir)/../output
		mv $(project_dir)/Build/$(bootloader)*/*/FV/UEFIPAYLOAD.fd $(project_dir)/../output/UEFIPAYLOAD.fd; \
		git checkout MdeModulePkg/Logo/Logo.bmp > /dev/null 2>&1 || true

clean:
	test -d $(project_dir) && (cd $(project_dir); rm -rf Build; rm -f Conf/tools_def.txt) || exit 0

distclean:
	rm -rf */

.PHONY: all update checktools config build clean distclean

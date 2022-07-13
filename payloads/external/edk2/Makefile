## SPDX-License-Identifier: GPL-2.0-only

# force the shell to bash - the edksetup.sh script doesn't work with dash
export SHELL := env bash

project_name = edk2
project_dir = $(CURDIR)/$(word 3,$(subst /, ,$(CONFIG_EDK2_REPOSITORY)))

BUILD_STR = -a IA32 -a X64 -t COREBOOT
BUILD_STR += -p UefiPayloadPkg/UefiPayloadPkg.dsc
BUILD_STR += -D BOOTLOADER=COREBOOT -q

#
# EDK II has the following build options relevant to coreboot:
#
#
# OPTION			= DEFAULT_VALUE
#
# ABOVE_4G_MEMORY		= TRUE
ifneq ($(CONFIG_EDK2_ABOVE_4G_MEMORY),y)
BUILD_STR += -D ABOVE_4G_MEMORY=FALSE
endif
# BOOTSPLASH_IMAGE		= FALSE
ifneq ($(CONFIG_EDK2_BOOTSPLASH_FILE),)
BUILD_STR += -D BOOTSPLASH_IMAGE=TRUE
endif
# BOOT_MANAGER_ESCAPE		= FALSE
ifeq ($(CONFIG_EDK2_BOOT_MANAGER_ESCAPE),y)
BUILD_STR += -D BOOT_MANAGER_ESCAPE=TRUE
endif
# BUILD_TARGETS			= DEBUG
ifeq ($(CONFIG_EDK2_RELEASE),y)
BUILD_STR += -b RELEASE
endif
# DISABLE_SERIAL_TERMINAL	= FALSE
ifneq ($(CONFIG_EDK2_SERIAL_SUPPORT),y)
BUILD_STR += -D DISABLE_SERIAL_TERMINAL=TRUE
endif
# FOLLOW_BGRT_SPEC		= FALSE
ifeq ($(CONFIG_EDK2_FOLLOW_BGRT_SPEC),y)
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
ifeq ($(CONFIG_EDK2_PRIORITIZE_INTERNAL),y)
BUILD_STR += -D PRIORITIZE_INTERNAL=TRUE
endif
# PS2_KEYBOARD_ENABLE		= FALSE
ifeq ($(CONFIG_EDK2_PS2_SUPPORT),y)
BUILD_STR += -D PS2_KEYBOARD_ENABLE=TRUE
endif
# PLATFORM_BOOT_TIMEOUT		= 3
ifneq ($(CONFIG_EDK2_BOOT_TIMEOUT),)
BUILD_STR += -D PLATFORM_BOOT_TIMEOUT=$(CONFIG_EDK2_BOOT_TIMEOUT)
endif
# SIO_BUS_ENABLE		= FALSE
ifeq ($(CONFIG_EDK2_PS2_SUPPORT),y)
BUILD_STR += -D SIO_BUS_ENABLE=TRUE
endif
# SHELL_TYPE			= BUILD_SHELL
ifneq ($(CONFIG_EDK2_HAVE_EFI_SHELL),y)
BUILD_STR += -D SHELL_TYPE=NONE
endif
# USE_CBMEM_FOR_CONSOLE		= FALSE
ifeq ($(CONFIG_EDK2_CBMEM_LOGGING),y)
BUILD_STR += -D USE_CBMEM_FOR_CONSOLE=TRUE
endif
# SD_MMC_TIMEOUT		= 1000000
ifneq ($(CONFIG_EDK2_SD_MMC_TIMEOUT),)
BUILD_STR += -D SD_MMC_TIMEOUT=$(shell echo $$(( $(CONFIG_EDK2_SD_MMC_TIMEOUT) * 1000)) )
endif

#
# EDKII has the below PCDs that are relevant to coreboot:
#
# Allows EDKII to use the full framebuffer
ifeq ($(CONFIG_EDK2_FULL_SCREEN_SETUP),y)
BUILD_STR += --pcd gEfiMdeModulePkgTokenSpaceGuid.PcdConOutRow=0
BUILD_STR += --pcd gEfiMdeModulePkgTokenSpaceGuid.PcdConOutColumn=0
BUILD_STR += --pcd gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutRow=0
BUILD_STR += --pcd gEfiMdeModulePkgTokenSpaceGuid.PcdSetupConOutColumn=0
endif

bootloader = $(word 8,$(subst /, ,$(BUILD_STR)))

ifneq ($(CONFIG_EDK2_CUSTOM_BUILD_PARAMS),)
BUILD_STR += $(CONFIG_EDK2_CUSTOM_BUILD_PARAMS)
endif

all: clean build

$(project_dir):
	echo "    Cloning $(project_name) from $(CONFIG_EDK2_REPOSITORY)"
	git clone $(CONFIG_EDK2_REPOSITORY) $(project_dir); \
		cd $(project_dir);

update: $(project_dir)
	if [ ! -d "$(project_dir)" ]; then \
		git clone $(CONFIG_EDK2_REPOSITORY) $(project_dir); \
	fi
	cd  $(project_dir); \
		echo "    Fetching new commits from $(CONFIG_EDK2_REPOSITORY)"; \
		git fetch origin 2>/dev/null; \
		if ! git rev-parse --verify -q $(CONFIG_EDK2_TAG_OR_REV) >/dev/null; then \
			echo "    $(CONFIG_EDK2_TAG_OR_REV) is not a valid git reference"; \
			exit 1; \
		fi; \
		if git status --ignore-submodules=dirty | grep -q clean; then \
			echo "    Checking out $(project_name) revision $(CONFIG_EDK2_TAG_OR_REV)"; \
			git checkout --detach $(CONFIG_EDK2_TAG_OR_REV) -f; \
		else \
			echo "    Working directory not clean; will not overwrite"; \
		fi; \
		git submodule update --init --checkout

logo: $(project_dir)
	case "$(CONFIG_EDK2_BOOTSPLASH_FILE)" in \
		"") ;; \
		/*) convert -background None $(CONFIG_EDK2_BOOTSPLASH_FILE) \
			BMP3:$(project_dir)/MdeModulePkg/Logo/Logo.bmp;; \
		*) convert -background None $(top)/$(CONFIG_EDK2_BOOTSPLASH_FILE) \
			BMP3:$(project_dir)/MdeModulePkg/Logo/Logo.bmp;; \
	esac \

checktools:
	echo -n "EDK2: Checking uuid-dev:"
	echo "#include <uuid/uuid.h>" > libtest.c
	echo "int main(int argc, char **argv) { (void) argc; (void) argv; return 0; }" >> libtest.c
	$(HOSTCC) $(HOSTCCFLAGS) libtest.c -o libtest >/dev/null 2>&1 && echo "    Found!" || \
		( echo "    Not found!"; \
			echo "ERROR: please_install uuid-dev (libuuid-devel)"; exit 1 )
	rm -rf libtest.c libtest
	echo -n "EDK2: Checking nasm:"
	type nasm > /dev/null 2>&1 && echo "        Found!" || \
		( echo "        Not found!"; echo "ERROR: Please install nasm."; exit 1 )
	echo -n "EDK2: Checking imagemagick:"
	-convert -size 1x1 xc: test.png &> /dev/null;
	if [ -f test.png ]; then \
		rm test.png && echo " Found!"; \
	else \
		echo " Not found!"; \
		echo "ERROR: Please install imagemagick"; \
		exit 1; \
	fi

build: update logo checktools
	echo "   ##### $(project_name) Build Summary #####"
	echo "   Repository:     $(CONFIG_EDK2_REPOSITORY)"
	echo "   Branch:         $(CONFIG_EDK2_TAG_OR_REV)"
	echo " $(BUILD_STR)" | \
		sed 's/-/\n   /g' | sort | sed \
		-e 's/a /Architecture:   /g' \
		-e 's/b /Release:        /g' \
		-e 's/D /Option:         /g' \
		-e 's/p /Payload:        /g' \
		-e 's/q /Build:          Quiet/' \
		-e 's/t /Toolchain:      /'
	unset CC; $(MAKE) -C $(project_dir)/BaseTools 2>&1
	cd $(project_dir); \
		export EDK_TOOLS_PATH=$(project_dir)/BaseTools; \
		export WORKSPACE=$(project_dir); \
		. ./edksetup.sh BaseTools; \
		grep -q "COREBOOT" $(project_dir)/Conf/tools_def.txt; \
		if [ $$? -ne 0 ]; then \
			cat ../tools_def.txt >> $(project_dir)/Conf/tools_def.txt; \
		fi; \
		build $(BUILD_STR); \
		mkdir -p $(project_dir)/../output; \
		mv $(project_dir)/Build/$(bootloader)*/*/FV/UEFIPAYLOAD.fd $(project_dir)/../output/UEFIPAYLOAD.fd; \
		git checkout MdeModulePkg/Logo/Logo.bmp > /dev/null 2>&1 || true

clean:
	test -d $(project_dir) && (cd $(project_dir); rm -rf Build; rm -f Conf/tools_def.txt) || exit 0

distclean:
	rm -rf */

.PHONY: all update checktools config build clean distclean logo

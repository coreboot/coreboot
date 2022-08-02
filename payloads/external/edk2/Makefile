## SPDX-License-Identifier: GPL-2.0-only

# force the shell to bash - the edksetup.sh script doesn't work with dash
export SHELL := env bash

project_name = edk2
export WORKSPACE := $(CURDIR)/workspace
export EDK2_PATH := $(WORKSPACE)/$(word 3,$(subst /, ,$(CONFIG_EDK2_REPOSITORY)))
export PACKAGES_PATH := $(EDK2_PATH)

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
ifeq ($(CONFIG_EDK2_DEBUG),y)
RELEASE_STR = DEBUG
else
RELEASE_STR = RELEASE
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

all: UefiPayloadPkg

$(WORKSPACE):
	mkdir $(WORKSPACE)

update: $(WORKSPACE)
	if [ ! -d "$(EDK2_PATH)" ]; then \
		git clone $(CONFIG_EDK2_REPOSITORY) $(EDK2_PATH); \
	fi
	cd $(EDK2_PATH); \
		git checkout MdeModulePkg/Logo/Logo.bmp > /dev/null 2>&1 || true; \
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

logo: update
	case "$(CONFIG_EDK2_BOOTSPLASH_FILE)" in \
		"") ;; \
		/*) convert -background None $(CONFIG_EDK2_BOOTSPLASH_FILE) \
			BMP3:$(EDK2_PATH)/MdeModulePkg/Logo/Logo.bmp;; \
		*) convert -background None $(top)/$(CONFIG_EDK2_BOOTSPLASH_FILE) \
			BMP3:$(EDK2_PATH)/MdeModulePkg/Logo/Logo.bmp;; \
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

print:
	echo "   ##### $(project_name) Build Summary #####"
	echo "   Repository:     $(CONFIG_EDK2_REPOSITORY)"
	echo "   Branch:         $(CONFIG_EDK2_TAG_OR_REV)"
	echo " $(BUILD_STR)" | \
		sed -e 's/--/-/g' -e 's/-/\n   /g' | sort | sed \
		-e 's/a /Architecture:   /g' \
		-e 's/b /Release:        /g' \
		-e 's/D /Option:         /g' \
		-e 's/pcd /Pcd:            /g' \
		-e 's/p /Payload:        /g' \
		-e 's/q /Build:          Quiet/' \
		-e 's/s /Build:          Silent/' \
		-e 's/t /Toolchain:      /'

prep: clean checktools logo update
	cd $(WORKSPACE); \
		source $(EDK2_PATH)/edksetup.sh; \
		unset CC; $(MAKE) -C $(EDK2_PATH)/BaseTools 2>&1; \
		grep -q "COREBOOT" $(EDK2_PATH)/Conf/tools_def.txt; \
		if [ $$? -ne 0 ]; then \
			cat ../tools_def.txt >> $(EDK2_PATH)/Conf/tools_def.txt; \
		fi; \

$(WORKSPACE)/Build/UefiPayloadPkgX64/$(RELEASE_STR)_COREBOOT/FV/UEFIPAYLOAD.fd: \
	prep print
	cd $(WORKSPACE); \
		source $(EDK2_PATH)/edksetup.sh; \
		echo -n "EDK2: Building... "; \
		build -b $(RELEASE_STR) $(BUILD_STR) \
			-y $(WORKSPACE)/Build/UefiPayloadPkgX64/UEFIPAYLOAD.txt; \
		if [ ! -f $@ ]; then \
			echo "Failed!"; \
			exit 1; \
		fi
		echo "Success!"; \

UefiPayloadPkg: $(WORKSPACE)/Build/UefiPayloadPkgX64/$(RELEASE_STR)_COREBOOT/FV/UEFIPAYLOAD.fd
	mv $(WORKSPACE)/Build/UefiPayloadPkgX64/$(RELEASE_STR)_COREBOOT/FV/UEFIPAYLOAD.fd \
		../../../build/UEFIPAYLOAD.fd

clean:
	test -d $(WORKSPACE) && (cd $(WORKSPACE); rm -rf Build; rm -f Conf/tools_def.txt) || exit 0

distclean:
	rm -rf $(WORKSPACE)

.PHONY: update logo checktools UefiPayloadPkg clean distclean

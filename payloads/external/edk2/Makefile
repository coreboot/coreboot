## SPDX-License-Identifier: GPL-2.0-only

# force the shell to bash - the edksetup.sh script doesn't work with dash
export SHELL := env bash

project_name = edk2
export WORKSPACE := $(CURDIR)/workspace
export EDK2_PATH := $(WORKSPACE)/$(word 3,$(subst /, ,$(CONFIG_EDK2_REPOSITORY)))

ifeq ($(CONFIG_EDK2_USE_EDK2_PLATFORMS),y)
export EDK2_PLATFORMS_PATH := $(WORKSPACE)/edk2-platforms
export PACKAGES_PATH := $(EDK2_PATH):\
			$(EDK2_PLATFORMS_PATH)/Platform/Intel:\
			$(EDK2_PLATFORMS_PATH)/Silicon/Intel:\
			$(EDK2_PLATFORMS_PATH)/Features/Intel:\
			$(EDK2_PLATFORMS_PATH)/Features/Intel/Debugging:\
			$(EDK2_PLATFORMS_PATH)/Features/Intel/Network:\
			$(EDK2_PLATFORMS_PATH)/Features/Intel/OutOfBandManagement:\
			$(EDK2_PLATFORMS_PATH)/Features/Intel/PowerManagement:\
			$(EDK2_PLATFORMS_PATH)/Features/Intel/SystemInformation:\
			$(EDK2_PLATFORMS_PATH)/Features/Intel/UserInterface
else
export PACKAGES_PATH := $(EDK2_PATH)
endif

OBJCOPY = $(GCC_PREFIX)objcopy

ifeq ($(CONFIG_EDK2_UEFIPAYLOAD),y)
BUILD_STR = -p UefiPayloadPkg/UefiPayloadPkg.dsc
endif
BUILD_STR += -t COREBOOT
BUILD_STR += -D BOOTLOADER=COREBOOT
ifneq ($(V),1)
BUILD_STR += -q
ifeq ($(CONFIG_EDK2_UEFIPAYLOAD),y)
BUILD_STR += -s
endif
endif

BUILD_STR += -D BUILD_ARCH=X64

#
# EDK II (edk2/master) has the following build options relevant to coreboot:
#
#
# OPTION			= DEFAULT_VALUE
#
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
# CPU_TIMER_LIB_ENABLE		= TRUE
ifneq ($(CONFIG_EDK2_CPU_TIMER_LIB),y)
BUILD_STR += -D CPU_TIMER_LIB_ENABLE=FALSE
else
BUILD_STR += --pcd gUefiCpuPkgTokenSpaceGuid.PcdCpuCoreCrystalClockFrequency=$(CONFIG_CPU_XTAL_HZ)
endif
# DISABLE_SERIAL_TERMINAL	= FALSE
ifneq ($(CONFIG_EDK2_SERIAL_SUPPORT),y)
BUILD_STR += -D DISABLE_SERIAL_TERMINAL=TRUE
endif
# VARIABLE_SUPPORT		= EMU
# MAX_VARIABLE_SIZE		= 0x10000
ifeq ($(CONFIG_SMMSTORE_V2),y)
BUILD_STR += -D VARIABLE_SUPPORT=SMMSTORE
BUILD_STR += --pcd gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize=0x8000
endif
# PCIE_BASE_ADDRESS		= 0
ifneq ($(CONFIG_ECAM_MMCONF_LENGTH),)
BUILD_STR += --pcd gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress=$(CONFIG_ECAM_MMCONF_BASE_ADDRESS)
endif
# PCIE_BASE_LENGTH		= 0
ifneq ($(CONFIG_ECAM_MMCONF_LENGTH),)
BUILD_STR += --pcd gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseSize=$(CONFIG_ECAM_MMCONF_LENGTH)
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
# EDK2_SECURE_BOOT_SUPPORT      = FALSE
ifeq ($(CONFIG_EDK2_SECURE_BOOT_SUPPORT), y)
BUILD_STR += -D SECURE_BOOT_ENABLE=TRUE
endif
# PCIEXP_SUPPORT_RESIZABLE_BARS = FALSE
ifeq ($(CONFIG_PCIEXP_SUPPORT_RESIZABLE_BARS), y)
BUILD_STR += --pcd gEfiMdeModulePkgTokenSpaceGuid.PcdPcieResizableBarSupport=TRUE
endif
# Preserve VRT bit in RTC_ADDRESS_REGISTER_D
# AMD SoC sets this every 1.024msec, but the generic code assumes it's read-only and
# hardcoded to 1 like on every Intel chipset since Intel ICH.
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2), y)
BUILD_STR += --pcd gPcAtChipsetPkgTokenSpaceGuid.PcdInitialValueRtcRegisterD=0x80
endif

#
# The MrChromebox repository has the following addition options:
#
ifeq ($(CONFIG_EDK2_REPO_MRCHROMEBOX),y)

# FOLLOW_BGRT_SPEC		= FALSE
ifeq ($(CONFIG_EDK2_FOLLOW_BGRT_SPEC),y)
BUILD_STR += -D FOLLOW_BGRT_SPEC=TRUE
endif
# GOP_DRIVER			= FALSE
ifeq ($(CONFIG_EDK2_GOP_DRIVER), y)
BUILD_STR += -D USE_PLATFORM_GOP=TRUE
endif
# PRIORITIZE_INTERNAL		= FALSE
ifeq ($(CONFIG_EDK2_PRIORITIZE_INTERNAL),y)
BUILD_STR += -D PRIORITIZE_INTERNAL=TRUE
endif
# TPM_ENABLE			= TRUE
ifeq ($(CONFIG_EDK2_DISABLE_TPM),y)
BUILD_STR += -D TPM_ENABLE=FALSE
endif
# UFS_ENABLE			= FALSE
ifeq ($(CONFIG_EDK2_UFS_ENABLE),y)
BUILD_STR += -D UFS_ENABLE=TRUE
endif
# USE_PCO_MMIO_EMMC		= FALSE
ifeq ($(CONFIG_EDK2_PCO_MMIO_EMMC),y)
BUILD_STR += -D USE_PCO_MMIO_EMMC=TRUE
endif
# TIMER_SUPPORT			= HPET
ifeq ($(CONFIG_EDK2_USE_LAPIC_TIMER),y)
BUILD_STR += -D TIMER_SUPPORT=LAPIC
endif
# LOAD_OPTION_ROMS		= FALSE
ifeq ($(CONFIG_EDK2_LOAD_OPTION_ROMS),y)
BUILD_STR += -D LOAD_OPTION_ROMS=TRUE
endif

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

BUILD_STR += --pcd gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemId="COREv4"

bootloader = $(word 8,$(subst /, ,$(BUILD_STR)))

ifneq ($(CONFIG_EDK2_CUSTOM_BUILD_PARAMS),)
BUILD_STR += $(CONFIG_EDK2_CUSTOM_BUILD_PARAMS)
endif

all: UefiPayloadPkg

$(WORKSPACE):
	mkdir $(WORKSPACE)

$(EDK2_PLATFORMS_PATH): $(WORKSPACE)
	if [ ! -d "$(EDK2_PLATFORMS_PATH)" ]; then \
		git clone --recurse-submodules $(CONFIG_EDK2_PLATFORMS_REPOSITORY) $(EDK2_PLATFORMS_PATH) -j5; \
	fi
	cd $(EDK2_PLATFORMS_PATH); \
		if ! git rev-parse --verify -q $(CONFIG_EDK2_PLATFORMS_TAG_OR_REV)^{object} >/dev/null; then \
			echo "    $(CONFIG_EDK2_PLATFORMS_TAG_OR_REV) is not a valid git reference"; \
			exit 1; \
		fi; \
		if git status --ignore-submodules=dirty | grep -q "nothing to commit, working tree clean"; then \
			echo "    Checking out edk2-platforms revision $(CONFIG_EDK2_PLATFORMS_TAG_OR_REV)"; \
			git checkout --detach $(CONFIG_EDK2_PLATFORMS_TAG_OR_REV) -f; \
		else \
			echo "    Working directory not clean; will not overwrite"; \
		fi; \
		git submodule update --init --checkout

$(EDK2_PATH): $(WORKSPACE)
	if [ ! -d "$(EDK2_PATH)" ]; then \
		git clone --recurse-submodules $(CONFIG_EDK2_REPOSITORY) $(EDK2_PATH) -j5; \
	fi
	cd $(EDK2_PATH); \
		git checkout MdeModulePkg/Logo/Logo.bmp > /dev/null 2>&1 || true; \
		if [ -e UefiPayloadPkg/ShimLayer/UniversalPayload.o ]; then \
			rm UefiPayloadPkg/ShimLayer/UniversalPayload.o; \
		fi; \
		echo "    Fetching new commits from $(CONFIG_EDK2_REPOSITORY)"; \
		git fetch origin 2>/dev/null; \
		if ! git rev-parse --verify -q $(CONFIG_EDK2_TAG_OR_REV)^{object} >/dev/null; then \
			echo "    $(CONFIG_EDK2_TAG_OR_REV) is not a valid git reference"; \
			exit 1; \
		fi; \
		if git status --ignore-submodules=dirty | grep -q -e clean -e "nothing added"; then \
			echo "    Checking out $(project_name) revision $(CONFIG_EDK2_TAG_OR_REV)"; \
			git checkout --detach $(CONFIG_EDK2_TAG_OR_REV) -f; \
			git submodule update --checkout --recursive; \
		else \
			echo "    Working directory not clean; will not overwrite"; \
		fi; \
		git submodule update --init --checkout

logo: $(EDK2_PATH)
	case "$(CONFIG_EDK2_BOOTSPLASH_FILE)" in \
		"") ;; \
		/*) convert -background None $(CONFIG_EDK2_BOOTSPLASH_FILE) \
			BMP3:$(EDK2_PATH)/MdeModulePkg/Logo/Logo.bmp;; \
		*) convert -background None $(top)/$(CONFIG_EDK2_BOOTSPLASH_FILE) \
			BMP3:$(EDK2_PATH)/MdeModulePkg/Logo/Logo.bmp;; \
	esac \

gop_driver: $(EDK2_PATH)
	if [ -n "$(CONFIG_EDK2_GOP_DRIVER)" ]; then \
		echo "Using GOP driver $(CONFIG_EDK2_GOP_FILE)"; \
		cp $(top)/$(CONFIG_EDK2_GOP_FILE) $(EDK2_PATH)/UefiPayloadPkg/IntelGopDriver.efi; \
		cp $(top)/$(CONFIG_INTEL_GMA_VBT_FILE) $(EDK2_PATH)/UefiPayloadPkg/vbt.bin; \
	fi; \

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
	echo "   Packages path:  $(PACKAGES_PATH)"
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

prep: $(EDK2_PATH) $(EDK2_PLATFORMS_PATH) clean checktools logo gop_driver
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
		build -a IA32 -a X64 -b $(RELEASE_STR) $(BUILD_STR) \
			-y $(WORKSPACE)/Build/UefiPayloadPkgX64/UEFIPAYLOAD.txt; \
		if [ ! -f $@ ]; then \
			echo "Failed!"; \
			exit 1; \
		fi
		echo "Success!"; \

$(WORKSPACE)/Build/UefiPayloadPkgX64/UniversalPayload.elf: \
	prep print
	cd $(WORKSPACE); \
		source $(EDK2_PATH)/edksetup.sh; \
		echo -n "EDK2: Building... "; \
		$(EDK2_PATH)/UefiPayloadPkg/UniversalPayloadBuild.sh -a IA32 -b $(RELEASE_STR) $(BUILD_STR)
		if [ ! -f $@ ]; then \
			echo "Failed!"; \
			exit 1; \
		fi
		echo "Success!"; \

$(WORKSPACE)/Build/UefiPayloadPkgX64/$(RELEASE_STR)_COREBOOT/IA32/UefiPayloadPkg/ShimLayer/ShimLayer/DEBUG/ShimLayer.dll: \
		$(WORKSPACE)/Build/UefiPayloadPkgX64/UniversalPayload.elf prep
	cd $(WORKSPACE)/Build/UefiPayloadPkgX64 && \
		$(OBJCOPY) -I binary UniversalPayload.elf -O elf32-i386 -B i386 \
			$(EDK2_PATH)/UefiPayloadPkg/ShimLayer/UniversalPayload.o; \
	cd $(WORKSPACE) && \
		source $(EDK2_PATH)/edksetup.sh; \
		build -p UefiPayloadPkg/UefiPayloadPkg.dsc -b $(RELEASE_STR) -a IA32 -a X64 \
			-m UefiPayloadPkg/ShimLayer/ShimLayer.inf \
			-t COREBOOT -D BOOTLOADER=COREBOOT -D SHIMLAYER=TRUE \
			-y $(WORKSPACE)/Build/UefiPayloadPkgX64/ShimLayer.txt

UefiPayloadPkg: $(WORKSPACE)/Build/UefiPayloadPkgX64/$(RELEASE_STR)_COREBOOT/FV/UEFIPAYLOAD.fd
	mv $(WORKSPACE)/Build/UefiPayloadPkgX64/$(RELEASE_STR)_COREBOOT/FV/UEFIPAYLOAD.fd \
		../../../build/UEFIPAYLOAD.fd

UniversalPayload: $(WORKSPACE)/Build/UefiPayloadPkgX64/$(RELEASE_STR)_COREBOOT/IA32/UefiPayloadPkg/ShimLayer/ShimLayer/DEBUG/ShimLayer.dll
	mv $(WORKSPACE)/Build/UefiPayloadPkgX64/$(RELEASE_STR)_COREBOOT/IA32/UefiPayloadPkg/ShimLayer/ShimLayer/DEBUG/ShimLayer.dll \
		../../../build/ShimmedUniversalPayload.elf

clean:
	test -d $(WORKSPACE) && (cd $(WORKSPACE); rm -rf Build; rm -f Conf/tools_def.txt) || exit 0

distclean:
	rm -rf $(WORKSPACE)

.PHONY: $(EDK2_PATH) checktools logo UefiPayloadPkg UniversalPayload clean distclean

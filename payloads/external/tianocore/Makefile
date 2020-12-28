## SPDX-License-Identifier: GPL-2.0-only

# force the shell to bash - the edksetup.sh script doesn't work with dash
export SHELL := env bash

project_name=Tianocore
project_dir=$(CURDIR)/tianocore
project_git_repo=https://github.com/mrchromebox/edk2
project_git_branch=coreboot_fb
upstream_git_repo=https://github.com/tianocore/edk2

ifeq ($(CONFIG_TIANOCORE_UEFIPAYLOAD),y)
bootloader=UefiPayloadPkg
logo_pkg=MdeModulePkg
build_flavor=-D BOOTLOADER=COREBOOT -D PCIE_BASE=$(CONFIG_MMCONF_BASE_ADDRESS) -DPS2_KEYBOARD_ENABLE
TAG=upstream/master
else
bootloader=CorebootPayloadPkg
logo_pkg=CorebootPayloadPkg
# STABLE revision is MrChromebox's coreboot framebuffer (coreboot_fb) branch
TAG=origin/$(project_git_branch)
endif

ifneq ($(CONFIG_TIANOCORE_REVISION_ID),)
TAG=$(CONFIG_TIANOCORE_REVISION_ID)
endif

export EDK_TOOLS_PATH=$(project_dir)/BaseTools

ifeq ($(CONFIG_TIANOCORE_DEBUG),y)
BUILD_TYPE=DEBUG
else
BUILD_TYPE=RELEASE
endif

ifneq ($(CONFIG_TIANOCORE_USE_8254_TIMER), y)
TIMER=-DUSE_HPET_TIMER
endif

TIMEOUT=-D PLATFORM_BOOT_TIMEOUT=$(CONFIG_TIANOCORE_BOOT_TIMEOUT)

ifeq ($(CONFIG_TIANOCORE_TARGET_IA32), y)
ARCH=-a IA32 -p $(bootloader)/$(bootloader)Ia32.dsc
else
ARCH=-a IA32 -a X64 -p $(bootloader)/$(bootloader)Ia32X64.dsc
endif

BUILD_STR=-q $(ARCH) -t COREBOOT -b $(BUILD_TYPE) $(TIMER) $(TIMEOUT) $(build_flavor)

all: clean build

$(project_dir):
	echo "    Cloning $(project_name) from Git"
	git clone --branch $(project_git_branch) $(project_git_repo) $(project_dir); \
		cd $(project_dir); \
		git remote add upstream $(upstream_git_repo)

update: $(project_dir)
	cd  $(project_dir); \
		echo "    Fetching new commits from the $(project_name) repo"; \
		git fetch --multiple origin upstream 2>/dev/null; \
		if ! git rev-parse --verify -q $(TAG) >/dev/null; then \
			echo "    $(TAG) is not a valid git reference"; \
			exit 1; \
		fi; \
		if git status --ignore-submodules=dirty | grep -qv clean; then \
			echo "    Checking out $(project_name) revision $(TAG)"; \
			git checkout --detach $(TAG); \
		else \
			echo "    Working directory not clean; will not overwrite"; \
		fi; \
		git submodule update --init

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
	unset CC; $(MAKE) -C $(project_dir)/BaseTools
	echo " build $(project_name) $(TAG)"
	if [ -n "$(CONFIG_TIANOCORE_BOOTSPLASH_FILE)" ]; then \
		echo "    Copying custom bootsplash image"; \
		case "$(CONFIG_TIANOCORE_BOOTSPLASH_FILE)" in \
			/*) cp $(CONFIG_TIANOCORE_BOOTSPLASH_FILE) \
				$(project_dir)/$(logo_pkg)/Logo/Logo.bmp;; \
			*) cp $(top)/$(CONFIG_TIANOCORE_BOOTSPLASH_FILE) \
				$(project_dir)/$(logo_pkg)/Logo/Logo.bmp;; \
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
		mv $(project_dir)/Build/$(bootloader)*/*/FV/UEFIPAYLOAD.fd $(project_dir)/Build/UEFIPAYLOAD.fd; \
		git checkout $(logo_pkg)/Logo/Logo.bmp > /dev/null 2>&1 || true

clean:
	test -d $(project_dir) && (cd $(project_dir); rm -rf Build; rm -f Conf/tools_def.txt) || exit 0

distclean:
	rm -rf $(project_dir)

.PHONY: all update checktools config build clean distclean

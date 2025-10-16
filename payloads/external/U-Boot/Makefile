## SPDX-License-Identifier: GPL-2.0-only

project_name=U-Boot
project_dir=u-boot
project_build_dir=build
project_config_file=$(project_build_dir)/.config

make_args=-C $(project_dir) O=../build

unexport KCONFIG_AUTOHEADER
unexport KCONFIG_AUTOCONFIG
unexport KCONFIG_DEPENDENCIES
unexport KCONFIG_SPLITCONFIG
unexport KCONFIG_TRISTATE
unexport KCONFIG_NEGATIVES
unexport $(COREBOOT_EXPORTS)

all: build

$(project_dir):
	echo "    Cloning $(project_name) from Git"
	git clone $(CONFIG_UBOOT_REPO_URL) -b $(CONFIG_UBOOT_BRANCH_OR_TAG) $(project_dir)

fetch: $(project_dir)
	echo "    Fetching new commits from the $(project_name) git repo"
	cd $(project_dir) && git fetch

checkout: fetch
	echo "    Checking out $(project_name) revision $(CONFIG_UBOOT_BRANCH_OR_TAG)"
	cd $(project_dir); git checkout $(CONFIG_UBOOT_BRANCH_OR_TAG); git branch -D coreboot 2>/dev/null; git checkout -b coreboot $(CONFIG_UBOOT_BRANCH_OR_TAG)

config: checkout
	mkdir -p $(project_build_dir)
	rm -f $(project_config_file)
ifneq ($(CONFIG_PAYLOAD_CONFIGFILE),)
ifneq ("$(wildcard $(CONFIG_PAYLOAD_CONFIGFILE))","")
	cat $(CONFIG_PAYLOAD_CONFIGFILE)" > tag-$(project_config_file)
	$(MAKE) $(make_args) olddefconfig
else
	echo "Error: File $(CONFIG_PAYLOAD_CONFIGFILE) does not exist"
	false
endif
else
	$(MAKE) $(make_args) coreboot_defconfig
endif

build: config
	echo "    MAKE       $(project_name) $(CONFIG_UBOOT_BRANCH_OR_TAG)"
	$(MAKE) $(make_args)

clean:
	test -d $(project_dir) && $(MAKE) $(make_args) clean || exit 0

distclean:
	rm -rf $(project_dir)

print-repo-info:
	echo "$(CONFIG_UBOOT_REPO_URL) $(project_dir)"

.PHONY: checkout config build clean distclean fetch print-repo-info

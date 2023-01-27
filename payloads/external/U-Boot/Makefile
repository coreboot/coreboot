## SPDX-License-Identifier: GPL-2.0-only

TAG-$(CONFIG_UBOOT_MASTER)=master
TAG-$(CONFIG_UBOOT_STABLE)=$(STABLE_COMMIT_ID)

project_name=U-Boot
project_dir=u-boot
project_git_repo=http://git.denx.de/u-boot.git
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
	git clone $(project_git_repo) -b $(TAG-y) $(project_dir)

fetch: $(project_dir)
ifeq ($(CONFIG_UBOOT_MASTER),y)
	echo "    Fetching new commits from the $(project_name) git repo"
	git fetch

	#master doesn't get a file, so it's continuously updated
	rm -f $(STABLE_COMMIT_ID)
else
	cd $(project_dir); git show $(TAG-y) >/dev/null 2>&1 ;	if [ $$? -ne 0 ]; \
	then echo "    Fetching new commits from the $(project_name) git repo"; git fetch; fi
	touch $(STABLE_COMMIT_ID)
endif

checkout: fetch
	echo "    Checking out $(project_name) revision $(TAG-y)"
	cd $(project_dir); git checkout master; git branch -D coreboot 2>/dev/null; git checkout -b coreboot $(TAG-y)

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
	echo "    MAKE       $(project_name) $(TAG-y)"
	$(MAKE) $(make_args)

clean:
	test -d $(project_dir) && $(MAKE) $(make_args) clean || exit 0

distclean:
	rm -rf $(project_dir)

print-repo-info:
	echo "$(project_git_repo) $(project_dir)"

.PHONY: checkout config build clean distclean fetch print-repo-info

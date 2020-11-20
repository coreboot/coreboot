## SPDX-License-Identifier: GPL-2.0-only

TAG-$(CONFIG_YABITS_MASTER)=origin/master
NAME-$(CONFIG_YABITS_MASTER)=Master
TAG-$(CONFIG_YABITS_STABLE)=d25abb067431dee9af9f8a874a209730ab7f0e91
NAME-$(CONFIG_YABITS_STABLE)=Stable
TAG-$(CONFIG_YABITS_REVISION)=$(CONFIG_YABITS_REVISION_ID)

project_name=Yabits
project_dir=$(CURDIR)/uefi
project_git_repo=https://github.com/yabits/uefi.git
LIBCONFIG_PATH="../../../libpayload"

all: build

$(project_dir):
	echo "    Cloning $(project_name) from Git"
	git clone $(project_git_repo) $(project_dir)

fetch: $(project_dir)
ifeq ($(TAG-y),)
	echo "Error: The specified tag is invalid"
ifeq ($(CONFIG_YABITS_REVISION),y)
	echo "Error: There is no revision specified for $(project_name)"
	false
endif
	false
endif
	-cd $(project_dir); git show $(TAG-y) >/dev/null 2>&1 ; \
		if [ $$? -ne 0 ] || [ "$(TAG-y)" = "origin/master" ]; then \
		echo "    Fetching new commits from the $(project_name) git repo"; \
		git fetch; fi

checkout: fetch
	echo "    Checking out $(project_name) revision $(NAME-y) ($(TAG-y))"
	cd $(project_dir); \
		git checkout master; \
		git branch -D coreboot 2>/dev/null; \
		git checkout -b coreboot $(TAG-y)

build: checkout
	echo "    MAKE       $(project_name) $(NAME-y)"
	$(if $(wildcard uefi/.xcompile),,$(shell bash ../../../util/xcompile/xcompile > uefi/.xcompile))
	$(MAKE) -C $(project_dir) defconfig LIBCONFIG_PATH=$(LIBCONFIG_PATH) XGCC=$(XGCCPATH)
	$(MAKE) -C $(project_dir) all LIBCONFIG_PATH=$(LIBCONFIG_PATH) XGCC=$(XGCCPATH)

clean:
	test -d $(project_dir) && $(MAKE) -C $(project_dir) clean LIBCONFIG_PATH=$(LIBCONFIG_PATH) XGCC=$(XGCCPATH) || exit 0

distclean:
	rm -rf $(project_dir)

print-repo-info:
	echo "$(project_git_repo) $(project_dir)"

.PHONY: all build checkout clean distclean fetch print-repo-info

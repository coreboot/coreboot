## SPDX-License-Identifier: GPL-2.0-only

project_name=Memtest86+

ifeq ($(CONFIG_MEMTEST86PLUS_V6),y)
project_dir=$(CURDIR)/memtest86plus_v6

ifeq ($(MEMTEST86PLUS_ARCH_64),y)
build_dir=$(project_dir)/build64
else
build_dir=$(project_dir)/build32
endif
project_git_repo=https://github.com/memtest86plus/memtest86plus.git
TAG-$(CONFIG_MEMTEST_MAIN)=origin/main
NAME-$(CONFIG_MEMTEST_MAIN)=Main
TAG-$(CONFIG_MEMTEST_STABLE)=v6.20
NAME-$(CONFIG_MEMTEST_STABLE)=Stable
TAG-$(CONFIG_MEMTEST_REVISION)=$(CONFIG_MEMTEST_REVISION_ID)
else
project_dir=$(CURDIR)/memtest86plus_v5

build_dir=$(project_dir)
project_git_repo=https://review.coreboot.org/memtest86plus.git
TAG-$(CONFIG_MEMTEST_MAIN)=origin/main
NAME-$(CONFIG_MEMTEST_MAIN)=Main
TAG-$(CONFIG_MEMTEST_STABLE)=0bd34c22604660e4283316331f3e7bf8a3863753
NAME-$(CONFIG_MEMTEST_STABLE)=Stable
TAG-$(CONFIG_MEMTEST_REVISION)=$(CONFIG_MEMTEST_REVISION_ID)
endif

all: build

$(project_dir):
	echo "    Cloning $(project_name) from Git"
	git clone $(project_git_repo) $(project_dir)

fetch: $(project_dir)
ifeq ($(TAG-y),)
	echo "Error: The specified tag is invalid"
ifeq ($(CONFIG_MEMTEST_REVISION),y)
	echo "Error: There is no revision specified for $(project_name)"
	false
endif
	false
endif
	-cd $(project_dir) 2>/dev/null; git show $(TAG-y) >/dev/null 2>&1 ; \
		if [ $$? -ne 0 ] || [ "$(TAG-y)" = "origin/main" ]; then \
		echo "    Fetching new commits from the $(project_name) git repo"; \
		git fetch; fi

checkout: fetch
	echo "    Checking out $(project_name) revision $(NAME-y) ($(TAG-y))"
	cd $(project_dir); \
		git checkout main; \
		git branch -D coreboot 2>/dev/null; \
		git checkout -b coreboot $(TAG-y)

build: checkout
	echo "    MAKE       $(project_name) $(NAME-y)"
	$(MAKE) -C $(build_dir) all
ifeq ($(CONFIG_MEMTEST86PLUS_V6),y)
	cp $(build_dir)/memtest.bin $(project_dir)/memtest
endif

clean:
	test -d $(build_dir) && $(MAKE) -C $(build_dir) clean || exit 0

distclean:
	rm -rf $(project_dir)

print-repo-info:
	echo "$(project_git_repo) $(project_dir)"

.PHONY: all build checkout check_version clean distclean fetch print-repo-info

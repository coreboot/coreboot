##
## This file is part of the coreboot project.
##
## Copyright (C) 2016 Google Inc.
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

TAG-$(CONFIG_MEMTEST_MASTER)=origin/master
NAME-$(CONFIG_MEMTEST_MASTER)=Master
TAG-$(CONFIG_MEMTEST_STABLE)=3754fd440f4009b62244e0f95c56bbb12c2fffcb
NAME-$(CONFIG_MEMTEST_STABLE)=Stable
TAG-$(CONFIG_MEMTEST_REVISION)=$(CONFIG_MEMTEST_REVISION_ID)

project_name=Memtest86+
project_dir=$(CURDIR)/memtest86plus
project_git_repo=https://review.coreboot.org/memtest86plus.git

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
	$(MAKE) -C $(project_dir) all

clean:
	test -d $(project_dir) && $(MAKE) -C $(project_dir) clean || exit 0

distclean:
	rm -rf $(project_dir)

print-repo-info:
	echo "$(project_git_repo) $(project_dir)"

.PHONY: all build checkout clean distclean fetch print-repo-info

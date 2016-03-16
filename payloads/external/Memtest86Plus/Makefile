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

project_name=Memtest86+
project_dir=$(CURDIR)/memtest86plus
project_git_repo=https://review.coreboot.org/memtest86plus

all: build

$(project_dir):
	echo "    Cloning $(project_name) from Git"
	git clone $(project_git_repo) $(project_dir)

fetch: $(project_dir)
	cd $(project_dir); \
		test -e '.git' && \
		git fetch && \
		git checkout origin/master

build: fetch
	echo "    MAKE       $(project_name)"
	$(MAKE) -C $(project_dir) all

clean:
	test -d $(project_dir) && $(MAKE) -C $(project_dir) clean || exit 0

distclean:
	rm -rf $(project_dir)

.PHONY: all build fetch clean distclean

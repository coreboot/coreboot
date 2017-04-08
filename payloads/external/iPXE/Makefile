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

# 2017.3 - Last commit of March 2017
# When updating, change the name both here and in payloads/external/iPXE/Kconfig
STABLE_COMMIT_ID=fd6d1f4660a37d75acba1c64e2e5f137307bbc31

TAG-$(CONFIG_IPXE_MASTER)=origin/master
TAG-$(CONFIG_IPXE_STABLE)=$(STABLE_COMMIT_ID)

project_name=iPXE
project_dir=ipxe
project_git_repo=https://git.ipxe.org/ipxe.git

all: build

$(project_dir):
	echo "    Cloning $(project_name) from Git"
	git clone $(project_git_repo) $(project_dir)

fetch: $(project_dir)
	cd $(project_dir); \
		git show $(TAG-y) >/dev/null 2>&1 ; \
		if [ $$? -ne 0 ] || [ "$(TAG-y)" = "origin/master" ]; then \
			echo "    Fetching new commits from the $(project_name) repo"; \
			git fetch; \
		fi

checkout: fetch
	echo "    Checking out $(project_name) revision $(TAG-y)"
	cd  $(project_dir); \
		git checkout master; \
		git branch -D coreboot 2>/dev/null; \
		git checkout -b coreboot $(TAG-y)

config: checkout
ifeq ($(CONSOLE_SERIAL),yy)
	cp "$(project_dir)/src/config/console.h" "$(project_dir)/src/config/console.h.cb"
	cp "$(project_dir)/src/config/serial.h" "$(project_dir)/src/config/serial.h.cb"
	sed 's|//#define\s*CONSOLE_SERIAL.*|#define CONSOLE_SERIAL|' "$(project_dir)/src/config/console.h" > "$(project_dir)/src/config/console.h.tmp"
	mv "$(project_dir)/src/config/console.h.tmp" "$(project_dir)/src/config/console.h"
	sed 's|#define\s*COMCONSOLE.*|#define COMCONSOLE $(IPXE_UART)|' "$(project_dir)/src/config/serial.h" > "$(project_dir)/src/config/serial.h.tmp"
	sed 's|#define\s*COMSPEED.*|#define COMSPEED $(CONFIG_TTYS0_BAUD)|'  "$(project_dir)/src/config/serial.h.tmp" > "$(project_dir)/src/config/serial.h"
endif

build: config
	echo "    MAKE       $(project_name) $(TAG-y)"
	$(MAKE) -C $(project_dir)/src bin/$(PXE_ROM_PCI_ID).rom
	cp $(project_dir)/src/bin/$(PXE_ROM_PCI_ID).rom $(project_dir)/ipxe.rom
ifeq ($(CONSOLE_SERIAL),yy)
	cp "$(project_dir)/src/config/console.h.cb" "$(project_dir)/src/config/console.h"
	cp "$(project_dir)/src/config/serial.h.cb" "$(project_dir)/src/config/serial.h"
endif

clean:
	test -d $(project_dir) && $(MAKE) -C $(project_dir)/src veryclean || exit 0
	rm -f $(project_dir)/ipxe.rom

distclean:
	rm -rf $(project_dir)

.PHONY: all fetch config build clean distclean

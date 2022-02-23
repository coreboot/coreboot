## SPDX-License-Identifier: GPL-2.0-only

# 2022.1 - Last commit of January 2022
# When updating, change the name both here and in payloads/external/iPXE/Kconfig
STABLE_COMMIT_ID=6ba671acd922ee046b257c5119b8a0f64d275473

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
ifneq ($(filter y,$(CONFIG_HAS_SCRIPT) $(CONFIG_PXE_NO_PROMPT)),)
	cp "$(project_dir)/src/config/general.h" "$(project_dir)/src/config/general.h.cb"
endif
ifeq ($(CONFIG_HAS_SCRIPT),y)
	sed 's|//#define\s*IMAGE_SCRIPT.*|#define IMAGE_SCRIPT|' "$(project_dir)/src/config/general.h" > "$(project_dir)/src/config/general.h.tmp"
	mv "$(project_dir)/src/config/general.h.tmp" "$(project_dir)/src/config/general.h"
endif
ifeq ($(CONFIG_PXE_NO_PROMPT),y)
	sed 's|#define\s*BANNER_TIMEOUT.*|#define BANNER_TIMEOUT 0|' "$(project_dir)/src/config/general.h" > "$(project_dir)/src/config/general.h.tmp"
	mv "$(project_dir)/src/config/general.h.tmp" "$(project_dir)/src/config/general.h"
endif
ifeq ($(CONFIG_PXE_HAS_HTTPS),y)
	sed 's|.*DOWNLOAD_PROTO_HTTPS|#define DOWNLOAD_PROTO_HTTPS|g' "$(project_dir)/src/config/general.h" > "$(project_dir)/src/config/general.h.tmp"
	mv "$(project_dir)/src/config/general.h.tmp" "$(project_dir)/src/config/general.h"
endif

build: config $(CONFIG_SCRIPT)
ifeq ($(CONFIG_HAS_SCRIPT),y)
	echo "    MAKE       $(project_name) $(TAG-y) EMBED=$(CONFIG_SCRIPT)"
	$(MAKE) -C $(project_dir)/src bin/$(PXE_ROM_PCI_ID).rom EMBED=$(CONFIG_SCRIPT)
else
	echo "    MAKE       $(project_name) $(TAG-y)"
	$(MAKE) -C $(project_dir)/src bin/$(PXE_ROM_PCI_ID).rom
endif
	cp $(project_dir)/src/bin/$(PXE_ROM_PCI_ID).rom $(project_dir)/ipxe.rom
ifeq ($(CONSOLE_SERIAL),yy)
	cp "$(project_dir)/src/config/console.h.cb" "$(project_dir)/src/config/console.h"
	cp "$(project_dir)/src/config/serial.h.cb" "$(project_dir)/src/config/serial.h"
endif
ifneq ($(filter y,$(CONFIG_HAS_SCRIPT) $(CONFIG_PXE_NO_PROMPT)),)
	cp "$(project_dir)/src/config/general.h.cb" "$(project_dir)/src/config/general.h"
endif

clean:
	test -d $(project_dir) && $(MAKE) -C $(project_dir)/src veryclean || exit 0
	rm -f $(project_dir)/ipxe.rom

distclean:
	rm -rf $(project_dir)

.PHONY: all fetch config build clean distclean

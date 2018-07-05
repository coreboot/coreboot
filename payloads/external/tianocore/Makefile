##
## This file is part of the coreboot project.
##
## Copyright (C) 2017 Google Inc.
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

# force the shell to bash - the edksetup.sh script doesn't work with dash
export SHELL := env bash

STABLE_COMMIT_ID=315d9d08fd77db1024ccc5307823da8aaed85e2f
TAG-$(CONFIG_TIANOCORE_MASTER)=origin/master
TAG-$(CONFIG_TIANOCORE_STABLE)=$(STABLE_COMMIT_ID)
TAG-$(CONFIG_TIANOCORE_REVISION)=$(CONFIG_TIANOCORE_REVISION_ID)

project_name=Tianocore
project_dir=$(CURDIR)/tianocore
project_git_repo=https://github.com/tianocore/edk2

export EDK_TOOLS_PATH=$(project_dir)/BaseTools

ifeq ($(CONFIG_TIANOCORE_DEBUG),y)
BUILD_TYPE=DEBUG
else
BUILD_TYPE=RELEASE
endif

ifeq ($(CONFIG_TIANOCORE_TARGET_IA32), y)
	BUILD_STR=-a IA32 -t COREBOOT -p CorebootPayloadPkg/CorebootPayloadPkgIa32.dsc -b $(BUILD_TYPE)
else
	BUILD_STR=-a IA32 -a X64 -t COREBOOT -p CorebootPayloadPkg/CorebootPayloadPkgIa32X64.dsc -b $(BUILD_TYPE)
endif

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

$(project_dir)/.version_$(TAG-y): fetch
	if  ! [[ -e $(project_dir)/.version_$(STABLE_COMMIT_ID) ]] || \
		[ "$(TAG-y)" = "origin/master" ] ; then \
		rm -f .version_*; \
		echo "    Checking out $(project_name) revision $(TAG-y)"; \
		cd  $(project_dir); \
			git checkout master; \
			git branch -D coreboot 2>/dev/null; \
			git checkout -b coreboot $(TAG-y); \
		for patch in $(CURDIR)/patches/*.patch; do \
			echo "Applying $$patch"; \
			cd $(project_dir); \
			git am --keep-cr $$patch || \
				( echo " Error when applying patches.\n"; git am --abort; exit 1; ); \
		done; \
		if ! [ "$(TAG-y)" = "origin/master" ] ; then \
			touch $(project_dir)/.version_$(STABLE_COMMIT_ID); \
		fi; \
	fi; \

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

build: $(project_dir)/.version_$(TAG-y) checktools
	unset CC; $(MAKE) -C $(project_dir)/BaseTools
	echo " build $(project_name) $(TAG-y)"
	cd $(project_dir); \
		export EDK_TOOLS_PATH=$(project_dir)/BaseTools; \
		export WORKSPACE=$(project_dir); \
		. ./edksetup.sh BaseTools; \
		grep -q "COREBOOT" $(project_dir)/Conf/tools_def.txt; \
		if [ $$? -ne 0 ]; then \
			cat ../tools_def.txt >> $(project_dir)/Conf/tools_def.txt; \
		fi; \
		build $(BUILD_STR); \
		mv $(project_dir)/Build/CorebootPayloadPkg*/*/FV/UEFIPAYLOAD.fd $(project_dir)/Build/UEFIPAYLOAD.fd

clean:
	test -d $(project_dir) && (cd $(project_dir); rm -rf Build; rm -f Conf/tools_def.txt) || exit 0

distclean:
	rm -rf $(project_dir)

.PHONY: all fetch checkout checktools config build clean distclean

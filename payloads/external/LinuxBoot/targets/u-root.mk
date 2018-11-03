## This file is part of the coreboot project.
##
## Copyright (C) 2017 Facebook Inc.
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

uroot_git_repo=https://github.com/u-root/u-root.git
uroot_dir=$(project_dir)/go/src/github.com/u-root/u-root
go_version=$(shell go version | sed -nr 's/.*go([0-9]+\.[0-9]+.?[0-9]?).*/\1/p' )
go_version_major=$(shell echo $(go_version) |  sed -nr 's/^([0-9]+)\.([0-9]+)\.?([0-9]*)$$/\1/p')
go_version_minor=$(shell echo $(go_version) |  sed -nr 's/^([0-9]+)\.([0-9]+)\.?([0-9]*)$$/\2/p')

project_dir=$(shell pwd)/linuxboot
project_name=u-root
go_path_dir=$(shell pwd)/linuxboot/go

all: build

check:
ifeq ("$(go_version)","")
	printf "\n<<Please install Golang >= 1.9 for u-root mode>>\n\n"
	exit 1
endif
ifeq ($(shell if [ $(go_version_major) -eq 1 ]; then echo y; fi),y)
ifeq ($(shell if [ $(go_version_minor) -lt 9 ]; then echo y; fi),y)
	printf "\n  Golang version $(go_version) currently installed.\n\
	<<Please install Golang >= 1.9 for u-root mode>>\n\n"
	exit 1
endif
endif
	mkdir -p $(project_dir)/go/src/github.com/u-root

$(uroot_dir)/.git:
	echo "    Git        Cloning u-root $(CONFIG_LINUXBOOT_UROOT_VERSION)"
	git clone $(uroot_git_repo) $(uroot_dir)

fetch: check $(uroot_dir)/.git
	-cd "$(uroot_dir)" && git fetch origin

checkout: fetch
	cd "$(uroot_dir)" && \
	if ! git diff --quiet _cb_checkout "$(CONFIG_LINUXBOOT_UROOT_VERSION)" -- 2>/dev/null; \
	then \
		printf "    CHECKOUT    $(project_name) [$(CONFIG_LINUXBOOT_UROOT_VERSION)]\n"; \
		git checkout $$(git rev-parse HEAD) >/dev/null 2>&1; \
		git branch -f _cb_checkout "$(CONFIG_LINUXBOOT_UROOT_VERSION)" && \
		git checkout _cb_checkout && \
		$(if $(project_patches), \
		for patch in $(project_patches); do \
			printf "    PATCH       $$patch\n"; \
			git am --keep-cr "$$patch" || \
				( printf "Error when applying patches.\n"; \
				  git am --abort; exit 1; ); \
		done;,true;) \
	fi

$(uroot_dir)/u-root: $(uroot_dir)/u-root.go
	echo "    GO        u-root"
	cd $(uroot_dir); GOPATH=$(go_path_dir) go build u-root.go

$(project_dir)/initramfs.cpio.xz: checkout $(uroot_dir)/u-root
	echo "    MAKE       u-root $(CONFIG_LINUXBOOT_UROOT_VERSION)"
ifneq ($(CONFIG_LINUXBOOT_UROOT_COMMANDS),)
ifneq ($(CONFIG_LINUXBOOT_UROOT_FILES),)
	cd $(uroot_dir); GOARCH=$(CONFIG_LINUXBOOT_ARCH) GOPATH=$(go_path_dir) ./u-root \
	-build=bb -files $(CONFIG_LINUXBOOT_UROOT_FILES) -o $(project_dir)/initramfs.cpio \
	$(patsubst %,cmds/%,$(CONFIG_LINUXBOOT_UROOT_COMMANDS))
else
	cd $(uroot_dir); GOARCH=$(CONFIG_LINUXBOOT_ARCH) GOPATH=$(go_path_dir) ./u-root \
	-build=bb -o $(project_dir)/initramfs.cpio \
	$(patsubst %,cmds/%,$(CONFIG_LINUXBOOT_UROOT_COMMANDS))
endif
else
ifneq ($(CONFIG_LINUXBOOT_UROOT_FILES),)
	cd $(uroot_dir); GOARCH=$(CONFIG_LINUXBOOT_ARCH) GOPATH=$(go_path_dir) ./u-root \
	-build=bb -files $(CONFIG_LINUXBOOT_UROOT_FILES) -o $(project_dir)/initramfs.cpio coreboot-app
else
	cd $(uroot_dir); GOARCH=$(CONFIG_LINUXBOOT_ARCH) GOPATH=$(go_path_dir) ./u-root \
	-build=bb -o $(project_dir)/initramfs.cpio coreboot-app
endif
endif
	xz -f --check=crc32 -9 --lzma2=dict=1MiB --threads=$(CPUS) $(project_dir)/initramfs.cpio

build: $(project_dir)/initramfs.cpio.xz

.PHONY: build checkout fetch all check

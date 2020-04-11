## SPDX-License-Identifier: GPL-2.0-only

project_dir=$(shell pwd)/linuxboot
go_path_dir=$(project_dir)/go
uroot_bin=$(project_dir)/u-root
uroot_package=github.com/u-root/u-root

ARCH-$(CONFIG_LIBUXBOOT_X86_64)=amd64
ARCH-$(CONFIG_LINUXBOOT_X86)=i386
ARCH-$(CONFIG_LINUXBOOT_ARM64)=arm64

go_version=$(shell go version | sed -nr 's/.*go([0-9]+\.[0-9]+.?[0-9]?).*/\1/p' )
go_version_major=$(shell echo $(go_version) |  sed -nr 's/^([0-9]+)\.([0-9]+)\.?([0-9]*)$$/\1/p')
go_version_minor=$(shell echo $(go_version) |  sed -nr 's/^([0-9]+)\.([0-9]+)\.?([0-9]*)$$/\2/p')

uroot_args+=-build=$(CONFIG_LINUXBOOT_UROOT_FORMAT)
uroot_args+=-initcmd $(CONFIG_LINUXBOOT_UROOT_INITCMD)
uroot_args+=-uinitcmd=$(CONFIG_LINUXBOOT_UROOT_UINITCMD)
uroot_args+=-defaultsh $(CONFIG_LINUXBOOT_UROOT_SHELL)
ifneq (CONFIG_LINUXBOOT_UROOT_FILES,)
uroot_args+=$(foreach file,$(CONFIG_LINUXBOOT_UROOT_FILES),-files $(PWD)/$(file))
endif

uroot_cmds=$(CONFIG_LINUXBOOT_UROOT_COMMANDS)

all: u-root

version:
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

get: version
	if [ -d "$(go_path_dir)/src/$(uroot_package)" ]; then \
	git -C $(go_path_dir)/src/$(uroot_package) checkout --quiet main; \
	git -C $(go_path_dir)/src/$(uroot_package) pull || \
	echo -e "\n<<Pulling u-root package from GitHub failed>>\n"; \
	else \
	git clone https://${uroot_package} ${go_path_dir}/src/${uroot_package} || \
	(echo -e "\n<<Failed to clone u-root package. Please check your internet access>>\n" && \
	exit 1); \
	fi

checkout: get
	git -C $(go_path_dir)/src/$(uroot_package) checkout --quiet $(CONFIG_LINUXBOOT_UROOT_VERSION)

build: checkout
	cd ${go_path_dir}/src/${uroot_package}; \
	go build -o ${uroot_bin} .

u-root: build
	GOARCH=$(ARCH-y) $(uroot_bin) \
	-uroot-source ${go_path_dir}/src/${uroot_package} \
	$(uroot_args) -o $(project_dir)/initramfs_u-root.cpio $(uroot_cmds)

.PHONY: all u-root build checkout get version

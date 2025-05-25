## SPDX-License-Identifier: GPL-2.0-only

uroot_package = github.com/u-root/u-root
uroot_build = build/go/src/$(uroot_package)

UROOT_ARCH-$(CONFIG_LINUXBOOT_X86_64)      = amd64
UROOT_ARCH-$(CONFIG_LINUXBOOT_X86)         = 386
UROOT_ARCH-$(CONFIG_LINUXBOOT_ARM64)       = arm64
UROOT_ARCH-$(CONFIG_LINUXBOOT_RISCV_RV64)  = riscv64

go_version = $(shell go version | sed -nr 's/.*go([0-9]+\.[0-9]+.?[0-9]?).*/\1/p' )
go_version_major = $(shell echo $(go_version) |  sed -nr 's/^([0-9]+)\.([0-9]+)\.?([0-9]*)$$/\1/p')
go_version_minor = $(shell echo $(go_version) |  sed -nr 's/^([0-9]+)\.([0-9]+)\.?([0-9]*)$$/\2/p')

uroot_args += -build=$(CONFIG_LINUXBOOT_UROOT_FORMAT)
uroot_args += -initcmd $(CONFIG_LINUXBOOT_UROOT_INITCMD)
uroot_args += -uinitcmd=$(CONFIG_LINUXBOOT_UROOT_UINITCMD)
uroot_args += -defaultsh $(CONFIG_LINUXBOOT_UROOT_SHELL)
ifneq (CONFIG_LINUXBOOT_UROOT_FILES,)
uroot_args += $(foreach file,$(CONFIG_LINUXBOOT_UROOT_FILES),-files $(PWD)/$(file))
endif

uroot_cmds = $(CONFIG_LINUXBOOT_UROOT_COMMANDS)

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

$(uroot_build): | build/
	git clone https://$(uroot_package) $(uroot_build)
	git -C $(uroot_build) checkout --quiet $(CONFIG_LINUXBOOT_UROOT_VERSION)

$(uroot_build)/u-root: | $(uroot_build)
	cd $(uroot_build); \
	go build -o u-root .

build/initramfs_u-root.cpio: $(uroot_build)/u-root
	cd $(uroot_build); \
	echo "GOARCH=$(UROOT_ARCH-y) ./u-root $(uroot_args) -o initramfs_u-root.cpio $(uroot_cmds)"
	cd $(uroot_build); \
	GOARCH=$(UROOT_ARCH-y) ./u-root $(uroot_args) -o initramfs_u-root.cpio $(uroot_cmds)
	cp $(uroot_build)/initramfs_u-root.cpio $@

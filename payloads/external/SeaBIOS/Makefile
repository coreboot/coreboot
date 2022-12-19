TAG-$(CONFIG_SEABIOS_MASTER)=origin/master
TAG-$(CONFIG_SEABIOS_STABLE)=3208b098f51a9ef96d0dfa71d5ec3a3eaec88f0a
TAG-$(CONFIG_SEABIOS_REVISION)=$(CONFIG_SEABIOS_REVISION_ID)

project_git_repo=https://review.coreboot.org/seabios.git
project_dir=seabios

unexport KCONFIG_AUTOHEADER
unexport KCONFIG_AUTOCONFIG
unexport KCONFIG_DEPENDENCIES
unexport KCONFIG_SPLITCONFIG
unexport KCONFIG_TRISTATE
unexport KCONFIG_NEGATIVES

all: build

seabios:
	echo "    Cloning SeaBIOS from Git"
	git clone $(project_git_repo) $(project_dir)

fetch: seabios
ifeq ($(TAG-y),)
	echo "Error: The specified tag is invalid"
ifeq ($(CONFIG_SEABIOS_REVISION),y)
	echo "Error: There is no revision specified for SeaBIOS"
	false
endif
	false
endif
	cd seabios; git show $(TAG-y) >/dev/null 2>&1 ;	if [ $$? -ne 0 ] || \
		[ "$(TAG-y)" = "origin/master" ]; then \
		echo "    Fetching new commits from the SeaBIOS git repo"; \
		git fetch; fi

checkout: fetch
	echo "    Checking out SeaBIOS revision $(TAG-y)"
	cd seabios; git checkout master; git branch -D coreboot 2>/dev/null; git checkout -b coreboot $(TAG-y)

config: checkout
	echo "    CONFIG     SeaBIOS $(TAG-y)"
	echo "CONFIG_COREBOOT=y" > seabios/.config
ifeq ($(CONFIG_CONSOLE_SERIAL)$(CONFIG_DRIVERS_UART_8250IO),yy)
	echo "CONFIG_DEBUG_SERIAL=y" >> seabios/.config
	echo "CONFIG_DEBUG_SERIAL_PORT=$(CONFIG_TTYS0_BASE)" >> seabios/.config
else ifeq ($(CONFIG_CONSOLE_SERIAL)$(CONFIG_DRIVERS_UART_8250MEM)$(CONFIG_HUDSON_UART),yyy)
	echo "CONFIG_DEBUG_SERIAL_MMIO=y" >> seabios/.config
	echo "CONFIG_DEBUG_SERIAL_MEM_ADDRESS=0xFEDC6000" >> seabios/.config
else ifeq ($(CONFIG_CONSOLE_SERIAL)$(CONFIG_ENABLE_HSUART),yy)
	echo "CONFIG_DEBUG_SERIAL_MMIO=y" >> seabios/.config
	echo "CONFIG_DEBUG_SERIAL_MEM_ADDRESS=$(CONFIG_CONSOLE_UART_BASE_ADDRESS)" >> seabios/.config
else ifeq ($(CONFIG_CONSOLE_SERIAL)$(CONFIG_DRIVERS_UART_8250MEM_32),yy)
	echo "CONFIG_DEBUG_SERIAL_MMIO=y" >> seabios/.config
	echo "CONFIG_DEBUG_SERIAL_MEM_ADDRESS=$(CONFIG_CONSOLE_UART_BASE_ADDRESS)" >> seabios/.config
else
	echo "# CONFIG_DEBUG_SERIAL is not set" >> seabios/.config
endif
ifneq ($(CONFIG_SEABIOS_THREAD_OPTIONROMS),y)
	echo "# CONFIG_THREAD_OPTIONROMS is not set" >> seabios/.config
endif
ifeq ($(CONFIG_SEABIOS_VGA_COREBOOT),y)
	echo "CONFIG_VGA_COREBOOT=y" >> seabios/.config
	echo "CONFIG_BUILD_VGABIOS=y" >> seabios/.config
endif
ifneq ($(CONFIG_PAYLOAD_CONFIGFILE),)
ifneq ("$(wildcard $(CONFIG_PAYLOAD_CONFIGFILE))","")
	cat $(CONFIG_PAYLOAD_CONFIGFILE) >> seabios/.config
else
	echo "Error: File $(CONFIG_PAYLOAD_CONFIGFILE) does not exist"
	false
endif
endif
ifneq ($(CONFIG_SEABIOS_DEBUG_LEVEL),-1)
	echo "CONFIG_DEBUG_LEVEL=$(CONFIG_SEABIOS_DEBUG_LEVEL)" >> seabios/.config
endif
ifneq ($(CONFIG_SEABIOS_HARDWARE_IRQ),y)
	echo "# CONFIG_HARDWARE_IRQ is not set" >> seabios/.config
endif
#	This shows how to force a previously set .config option *off*
#	echo "# CONFIG_SMBIOS is not set" >> seabios/.config
	$(MAKE) -C seabios olddefconfig OUT=out/

build: config
	echo "    MAKE       SeaBIOS $(TAG-y)"
	$(MAKE) -C seabios OUT=out/

clean:
	test -d seabios/out && rm -rf seabios/out || exit 0

distclean:
	rm -rf seabios

print-repo-info:
	echo "$(project_git_repo) $(project_dir)"

.PHONY: checkout config build clean distclean clone fetch print-repo-info

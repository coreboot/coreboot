TAG-$(CONFIG_SEABIOS_MASTER)=origin/master
TAG-$(CONFIG_SEABIOS_STABLE)=e51488c5f8800a52ac5c8da7a31b85cca5cc95d2

unexport KCONFIG_AUTOHEADER
unexport KCONFIG_AUTOCONFIG
unexport KCONFIG_DEPENDENCIES
unexport KCONFIG_SPLITCONFIG
unexport KCONFIG_TRISTATE
unexport KCONFIG_NEGATIVES

all: build

seabios:
	echo "    Cloning SeaBIOS from Git"
	git clone http://review.coreboot.org/p/seabios.git seabios

fetch: seabios
	cd seabios; git show $(TAG-y) >/dev/null 2>&1 ;	if [ $$? -ne 0 ]; \
	then echo "    Fetching new commits from the SeaBIOS git repo"; git fetch; fi

checkout: fetch
	echo "    Checking out SeaBIOS revision $(TAG-y)"
	cd seabios; git checkout master; git branch -D coreboot 2>/dev/null; git checkout -b coreboot $(TAG-y)

config: checkout
	echo "    CONFIG     SeaBIOS $(TAG-y)"
	echo "CONFIG_COREBOOT=y" > seabios/.config
ifeq ($(CONFIG_CONSOLE_SERIAL),y)
	echo "CONFIG_DEBUG_SERIAL=y" >> seabios/.config
	echo "CONFIG_DEBUG_SERIAL_PORT=$(CONFIG_TTYS0_BASE)" >> seabios/.config
else
	echo "# CONFIG_DEBUG_SERIAL is not set" >> seabios/.config
endif
ifneq ($(CONFIG_SEABIOS_MALLOC_UPPERMEMORY),y)
	echo "# CONFIG_MALLOC_UPPERMEMORY is not set" >> seabios/.config
endif
ifneq ($(CONFIG_SEABIOS_THREAD_OPTIONROMS),y)
	echo "# CONFIG_THREAD_OPTIONROMS is not set" >> seabios/.config
endif
ifeq ($(CONFIG_SEABIOS_VGA_COREBOOT),y)
	echo "CONFIG_VGA_COREBOOT=y" >> seabios/.config
	echo "CONFIG_BUILD_VGABIOS=y" >> seabios/.config
endif
	# This shows how to force a previously set .config option *off*
	#echo "# CONFIG_SMBIOS is not set" >> seabios/.config
	$(MAKE) -C seabios olddefconfig OUT=out/

build: config
	echo "    MAKE       SeaBIOS $(TAG-y)"
	$(MAKE) -C seabios OUT=out/

clean:
	test -d seabios && $(MAKE) -C seabios clean OUT=out/ || exit 0

distclean:
	rm -rf seabios

.PHONY: checkout config build clean distclean clone fetch

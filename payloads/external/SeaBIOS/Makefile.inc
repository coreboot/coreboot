

TAG-$(CONFIG_SEABIOS_MASTER)=origin/master
TAG-$(CONFIG_SEABIOS_STABLE)=1efb10b9ea30c45a8c9c6230234fefa10d2886ed

unexport KCONFIG_AUTOCONFIG

all: build

seabios:
	echo "    Cloning SeaBIOS from Git"
	git clone git://git.seabios.org/seabios.git

fetch: seabios
	cd seabios; git show $(TAG-y) >/dev/null 2>&1 ;	if [ $$? -ne 0 ]; \
	then echo "    Fetching new commits from the SeaBIOS git repo"; git fetch; fi

checkout: fetch
	echo "    Checking out SeaBIOS revision $(TAG-y)"
	cd seabios; git checkout master; git branch -D coreboot 2>/dev/null; git checkout -b coreboot $(TAG-y)

config: checkout
	echo "    CONFIG     SeaBIOS $(TAG-y)"
	$(MAKE) -C seabios defconfig
	echo "CONFIG_COREBOOT=y" >> seabios/.config
	echo "CONFIG_DEBUG_SERIAL=y" >> seabios/.config
	echo "CONFIG_COREBOOT_FLASH=y" >> seabios/.config
	echo "CONFIG_LZMA=y" >> seabios/.config
	echo "CONFIG_FLASH_FLOPPY=y" >> seabios/.config
	echo "CONFIG_VGAHOOKS=y" >> seabios/.config
	# This shows how to force a previously set .config option *off*
	#echo "# CONFIG_SMBIOS is not set" >> seabios/.config

build: config
	echo "    MAKE       SeaBIOS $(TAG-y)"
	$(MAKE) -C seabios

clean:
	test -d seabios && $(MAKE) -C seabios clean || exit 0

distclean:
	rm -rf seabios

.PHONY: checkout config build clean distclean clone fetch



TAG-$(CONFIG_SEABIOS_MASTER)=origin/master
TAG-$(CONFIG_SEABIOS_STABLE)=88cb66ea542906ffff8a80ef397b9e3adbb33116

unexport KCONFIG_AUTOCONFIG

all: build

$(OUT)/seabios:
	echo "    Cloning SeaBIOS from Git"
	git clone http://review.coreboot.org/p/seabios.git $(OUT)/seabios

fetch: $(OUT)/seabios
	cd $(OUT)/seabios; git show $(TAG-y) >/dev/null 2>&1 ;	if [ $$? -ne 0 ]; \
	then echo "    Fetching new commits from the SeaBIOS git repo"; git fetch; fi

checkout: fetch
	echo "    Checking out SeaBIOS revision $(TAG-y)"
	cd $(OUT)/seabios; git checkout master; git branch -D coreboot 2>/dev/null; git checkout -b coreboot $(TAG-y)

config: checkout
	echo "    CONFIG     SeaBIOS $(TAG-y)"
	$(MAKE) -C $(OUT)/seabios defconfig OUT=$(OUT)/seabios/out/
	echo "CONFIG_COREBOOT=y" >> $(OUT)/seabios/.config
	echo "CONFIG_DEBUG_SERIAL=y" >> $(OUT)/seabios/.config
	echo "CONFIG_DEBUG_SERIAL_PORT=0x3f8" >> $(OUT)/seabios/.config
	echo "CONFIG_COREBOOT_FLASH=y" >> $(OUT)/seabios/.config
	echo "CONFIG_LZMA=y" >> $(OUT)/seabios/.config
	echo "CONFIG_FLASH_FLOPPY=y" >> $(OUT)/seabios/.config
	echo "CONFIG_VGAHOOKS=y" >> $(OUT)/seabios/.config
	# This shows how to force a previously set .config option *off*
	#echo "# CONFIG_SMBIOS is not set" >> $(OUT)/seabios/.config

build: config
	echo "    MAKE       SeaBIOS $(TAG-y)"
	$(MAKE) -C $(OUT)/seabios OUT=$(OUT)/seabios/out/

clean:
	test -d $(OUT)/seabios && $(MAKE) -C $(OUT)/seabios clean OUT=$(OUT)/seabios/out/ || exit 0

distclean:
	rm -rf $(OUT)/seabios

.PHONY: checkout config build clean distclean clone fetch

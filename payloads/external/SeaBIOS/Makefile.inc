TAG-$(CONFIG_SEABIOS_MASTER)=origin/master
TAG-$(CONFIG_SEABIOS_STABLE)=96917a8ed761f017fc8c72ba3b9181fbac03ac59

unexport KCONFIG_AUTOHEADER
unexport KCONFIG_AUTOCONFIG
unexport KCONFIG_DEPENDENCIES
unexport KCONFIG_SPLITCONFIG
unexport KCONFIG_TRISTATE
unexport KCONFIG_NEGATIVES

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
	echo "CONFIG_COREBOOT=y" > $(OUT)/seabios/.config
ifeq ($(CONFIG_CONSOLE_SERIAL),y)
	echo "CONFIG_DEBUG_SERIAL=y" >> $(OUT)/seabios/.config
	echo "CONFIG_DEBUG_SERIAL_PORT=$(CONFIG_TTYS0_BASE)" >> $(OUT)/seabios/.config
else
	echo "# CONFIG_DEBUG_SERIAL is not set" >> $(OUT)/seabios/.config
endif
ifneq ($(CONFIG_SEABIOS_THREAD_OPTIONROMS),y)
	echo "# CONFIG_THREAD_OPTIONROMS is not set" >> $(OUT)/seabios/.config
endif
	# This shows how to force a previously set .config option *off*
	#echo "# CONFIG_SMBIOS is not set" >> $(OUT)/seabios/.config
	$(MAKE) -C $(OUT)/seabios olddefconfig OUT=$(OUT)/seabios/out/

build: config
	echo "    MAKE       SeaBIOS $(TAG-y)"
	$(MAKE) -C $(OUT)/seabios OUT=$(OUT)/seabios/out/

clean:
	test -d $(OUT)/seabios && $(MAKE) -C $(OUT)/seabios clean OUT=$(OUT)/seabios/out/ || exit 0

distclean:
	rm -rf $(OUT)/seabios

.PHONY: checkout config build clean distclean clone fetch

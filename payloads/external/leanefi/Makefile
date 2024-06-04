
unexport KCONFIG_AUTOHEADER
unexport KCONFIG_AUTOCONFIG
unexport KCONFIG_DEPENDENCIES
unexport KCONFIG_SPLITCONFIG
unexport KCONFIG_TRISTATE
unexport KCONFIG_NEGATIVES
unexport $(COREBOOT_EXPORTS)

build: leanefi
	$(MAKE) -C leanefi

leanefi:
	git clone "https://review.coreboot.org/leanefi"

distclean:
	rm -rf leanefi

clean:
	rm -rf leanefi/build

.PHONY: build clean distclean

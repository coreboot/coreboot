TAG-$(CONFIG_FILO_MASTER)=
NAME-$(CONFIG_FILO_MASTER)=HEAD
TAG-$(CONFIG_FILO_STABLE)=4dbb31a64fe5b1c7e3025ab34619220609897646
NAME-$(CONFIG_FILO_STABLE)=STABLE

unexport KCONFIG_AUTOCONFIG

all: filo

checkout:
	echo "    GIT        FILO $(NAME-y)"
	test -d filo || \
		git clone http://review.coreboot.org/p/filo.git
	cd filo && \
		git checkout master && \
		git pull; \
		test -n $(TAG-y) && \
			git branch -f $(NAME-y) $(TAG-y) && \
			git checkout $(NAME-y)

config: libpayload
	echo "    CONFIG     FILO $(NAME-y)"
	$(MAKE) -C filo defconfig LIBCONFIG_PATH=../../../libpayload
	echo "CONFIG_COREBOOT=y" >> filo/.config
	echo "CONFIG_DEBUG_SERIAL=y" >> filo/.config
	echo "CONFIG_COREBOOT_FLASH=y" >> filo/.config
	echo "CONFIG_LZMA=y" >> filo/.config
	echo "CONFIG_FLASH_FLOPPY=y" >> filo/.config
	echo "CONFIG_VGAHOOKS=y" >> filo/.config
	# This shows how to force a previously set .config option *off*
	#echo "# CONFIG_SMBIOS is not set" >> filo/.config

filo: config
	echo "    MAKE       FILO $(NAME-y)"
	$(MAKE) -C filo

libpayload: checkout
	cd ../../libpayload && $(MAKE) defconfig && \
		$(MAKE) && $(MAKE) DESTDIR=../external/FILO/filo/build install

clean:
	test -d filo && $(MAKE) -C filo clean || exit 0

distclean:
	rm -rf filo

.PHONY: checkout config filo clean distclean


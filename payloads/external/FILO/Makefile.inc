TAG-$(CONFIG_FILO_MASTER)=origin/master
NAME-$(CONFIG_FILO_MASTER)=MASTER
TAG-$(CONFIG_FILO_STABLE)=4dbb31a64fe5b1c7e3025ab34619220609897646
NAME-$(CONFIG_FILO_STABLE)=STABLE

unexport KCONFIG_AUTOHEADER
unexport KCONFIG_AUTOCONFIG
unexport KCONFIG_DEPENDENCIES
unexport KCONFIG_SPLITCONFIG
unexport KCONFIG_TRISTATE
unexport KCONFIG_NEGATIVES

all: filo

checkout:
	echo "    GIT        FILO $(NAME-y)"
	test -d filo || \
		git clone http://review.coreboot.org/p/filo.git
	cd filo && \
		git checkout master && \
		git remote update && \
		test -n $(TAG-y) && \
			git branch -f $(NAME-y) $(TAG-y) && \
			git checkout $(NAME-y)

config: libpayload
	echo "    CONFIG     FILO $(NAME-y)"
	$(MAKE) -C filo defconfig LIBCONFIG_PATH=../../../libpayload
	# This shows how to force a previously unset .config option *on*
	#echo "CONFIG_VGAHOOKS=y" >> filo/.config
	# This shows how to force a previously set .config option *off*
	#echo "# CONFIG_SMBIOS is not set" >> filo/.config
	$(MAKE) -C filo oldconfig LIBCONFIG_PATH=../../../libpayload

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

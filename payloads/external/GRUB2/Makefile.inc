TAG-$(CONFIG_GRUB2_MASTER)=
NAME-$(CONFIG_GRUB2_MASTER)=HEAD

unexport KCONFIG_AUTOCONFIG
unexport CFLAGS
unexport CPPFLAGS
unexport CCASFLAGS
unexport CC
unexport BUILD_CC
unexport TARGET_CC
unexport TARGET_CFLAGS
unexport TARGET_CPPFLAGS
unexport TARGET_STRIP
unexport TARGET_OBJCOPY
unexport HOST_CFLAGS
unexport HOST_CPPFLAGS
unexport HOST_CC


all: grub2

checkout:
	echo "    GIT        GRUB2 $(NAME-y)"
	test -d grub2 || \
		git clone git://git.sv.gnu.org/grub.git grub2
	cd grub2 && \
		git checkout master && \
		git pull; \
		test -n "$(TAG-y)" && \
			git branch -f $(NAME-y) $(TAG-y) && \
			git checkout $(NAME-y) || true

config: checkout
	echo "    CONFIG     GRUB2 $(NAME-y)"
	rm -rf grub2/build
	mkdir grub2/build
	cd grub2 && ./autogen.sh
	cd grub2/build && ../configure BUILD_CC="$(HOSTCC)" CC="$(HOSTCC)" \
	TARGET_CC="$(CC)" \
	TARGET_OBJCOPY="$(OBJCOPY)" TARGET_STRIP="$(STRIP)" CFLAGS=-O2 TARGET_CFLAGS=-Os --with-platform=coreboot

grub2: config
	echo "    MAKE       GRUB2 $(NAME-y)"
	$(MAKE) -C grub2/build CC="$(HOSTCC)"
	$(MAKE) -C grub2/build default_payload.elf

clean:
	test -d grub2 && $(MAKE) -C grub2 clean || exit 0

distclean:
	rm -rf grub2

.PHONY: checkout config grub2 clean distclean

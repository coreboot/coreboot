TAG-$(CONFIG_GRUB2_MASTER)=
TAG-$(CONFIG_GRUB2_REVISION)=$(CONFIG_GRUB2_REVISION_ID)
TAG-$(CONFIG_GRUB2_STABLE)=e54c99aaff5e5f6f5d3b06028506c57e66d8ef77
NAME-$(CONFIG_GRUB2_MASTER)=HEAD
NAME-$(CONFIG_GRUB2_REVISION)=$(CONFIG_GRUB2_REVISION_ID)
NAME-$(CONFIG_GRUB2_STABLE)=2.02

project_git_repo=https://git.savannah.gnu.org/git/grub.git/
project_dir=grub2

unexport HOSTCC CC LD OBJCOPY STRIP
MAKEOVERRIDES :=

all: grub2

checkout:
	echo "    GIT        GRUB2 $(NAME-y)"
	test -d grub2 || \
		git clone $(project_git_repo) $(project_dir)
	cd grub2 && \
		git checkout master && \
		git pull; \
		test -n "$(TAG-y)" && \
			git branch -f $(NAME-y) $(TAG-y) && \
			git checkout $(NAME-y) || true

grub2/build/config.h: $(CONFIG_DEP) | checkout
	echo "    CONFIG     GRUB2 $(NAME-y)"
	rm -rf grub2/build
	mkdir grub2/build
	cd grub2 && ./autogen.sh
	cd grub2/build && ../configure CC="$(HOSTCC)" LD="$(LD)" \
	FREETYPE="pkg-config freetype2" BUILD_FREETYPE="pkg-config freetype2" \
	TARGET_CC="$(CC)" TARGET_OBJCOPY="$(OBJCOPY)" TARGET_STRIP="$(STRIP)" \
	CFLAGS=-O2 TARGET_CFLAGS=-Os \
	--with-platform=coreboot --enable-boot-time --disable-werror

config: grub2/build/config.h checkout

grub2: config
	echo "    MAKE       GRUB2 $(NAME-y)"
	$(MAKE) -C grub2/build
	$(MAKE) -C grub2/build default_payload.elf \
		EXTRA_PAYLOAD_MODULES="$(CONFIG_GRUB2_EXTRA_MODULES)"

clean:
	test -f grub2/build/Makefile && $(MAKE) -C grub2/build clean || exit 0

distclean:
	rm -rf grub2

print-repo-info:
	echo "$(project_git_repo) $(project_dir)"

.PHONY: checkout config grub2 clean distclean print-repo-info

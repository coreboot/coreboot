project_git_repo=https://gitlab.com/bztsrc/bootboot.git
project_dir=bootboot
ifeq ($(CONFIG_COREBOOT_BUILD),)
include ../../../.config
endif
ifeq ($(CONFIG_ARCH_ARM64),y)
loader_dir=$(project_dir)/aarch64-cb
else
loader_dir=$(project_dir)/x86_64-cb
endif

unexport KCONFIG_AUTOHEADER
unexport KCONFIG_AUTOCONFIG
unexport KCONFIG_DEPENDENCIES
unexport KCONFIG_SPLITCONFIG
unexport KCONFIG_TRISTATE
unexport KCONFIG_NEGATIVES

all: bootboot

checkout:
	echo "    GIT        BOOTBOOT $(loader_dir)"
	test -L $(project_dir) || test -d $(project_dir) || \
		git clone $(project_git_repo) $(project_dir)

bootboot: libpayload
	echo "    MAKE       $(loader_dir)"
	$(MAKE) -C $(loader_dir) LIBCONFIG_PATH=../../../libpayload

libpayload: checkout
	cp $(loader_dir)/lib.config ../../libpayload/.config
	cd ../../libpayload && $(MAKE) oldconfig && \
		$(MAKE) && $(MAKE) DESTDIR=../external/BOOTBOOT/$(loader_dir) install

clean:
	test -d $(loader_dir) && $(MAKE) -C $(loader_dir) clean || exit 0

distclean:
	rm -rf $(project_dir)

print-repo-info:
	echo "$(project_git_repo) $(project_dir)"

.PHONY: checkout bootboot libpayload clean distclean print-repo-info

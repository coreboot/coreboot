## SPDX-License-Identifier: GPL-2.0-only
project_git_repo=https://github.com/nic3-14159/coreDOOM.git
project_dir=coredoom

unexport KCONFIG_AUTOHEADER
unexport KCONFIG_AUTOCONFIG
unexport KCONFIG_DEPENDENCIES
unexport KCONFIG_SPLITCONFIG
unexport KCONFIG_TRISTATE
unexport KCONFIG_NEGATIVES

all: coredoom

checkout:
	test -d coredoom || \
	    git clone $(project_git_repo) $(project_dir)
	cd $(project_dir) && \
	    git checkout libpayload_port

coredoom: libpayload
	$(MAKE) -C $(project_dir)/doomgeneric

libpayload: checkout
	cp libpayload-config ../../libpayload/.config && \
	    cd ../../libpayload && $(MAKE) olddefconfig && $(MAKE) && \
	    $(MAKE) DESTDIR=../external/coreDOOM/coredoom/doomgeneric install

clean:
	test -d coredoom && $(MAKE) -C coredoom/doomgeneric clean || exit 0

distclean:
	rm -rf coredoom

.PHONY: checkout coredoom clean distclean

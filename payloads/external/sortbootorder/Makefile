project_url=https://github.com/pcengines/sortbootorder/archive/master.tar.gz
archive_name=master.tar.gz

unexport KCONFIG_AUTOHEADER
unexport KCONFIG_AUTOCONFIG
unexport KCONFIG_DEPENDENCIES
unexport KCONFIG_SPLITCONFIG
unexport KCONFIG_TRISTATE
unexport KCONFIG_NEGATIVES

all: sortbootorder

#sortbootorder: patch
sortbootorder:
	echo "    MAKE       sortbootorder     "
	$(MAKE) -C sortbootorder

#patch: download
#	cd sortbootorder; \
#	if [ -e debian ]; then patch -l -p1 < ../libpayload_sortbootorder.patch; fi

download:
	test -d sortbootorder || { wget $(project_url); \
		tar -xvf $(archive_name); \
		rm $(archive_name); \
		mv sortbootorder-master sortbootorder; }

clean:
	test -d sortbootorder && $(MAKE) -C sortbootorder clean || exit 0

distclean:
	rm -rf sortbootorder

.PHONY: download patch sortbootorder clean distclean

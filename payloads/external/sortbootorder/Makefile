version=4.5.4
branch_name=v$(version)
project_url=https://github.com/pcengines/sortbootorder/archive/$(branch_name).tar.gz
archive_name=$(branch_name).tar.gz

unexport KCONFIG_AUTOHEADER
unexport KCONFIG_AUTOCONFIG
unexport KCONFIG_DEPENDENCIES
unexport KCONFIG_SPLITCONFIG
unexport KCONFIG_TRISTATE
unexport KCONFIG_NEGATIVES

all: sortbootorder

sortbootorder: download
	echo "    MAKE       sortbootorder     "
	$(MAKE) -C sortbootorder

download:
	test -d sortbootorder || { wget $(project_url); \
		tar -xvf $(archive_name); \
		rm $(archive_name); \
		mv sortbootorder-$(version) sortbootorder; }

clean:
	test -d sortbootorder && $(MAKE) -C sortbootorder clean || exit 0

distclean:
	rm -rf sortbootorder

.PHONY: download patch sortbootorder clean distclean

project_url=http://snapshot.debian.org/archive/debian-archive/20110127T084257Z/debian/pool/main/t/tint/tint_0.03b.tar.gz
archive_name=tint_0.03b.tar.gz

unexport KCONFIG_AUTOHEADER
unexport KCONFIG_AUTOCONFIG
unexport KCONFIG_DEPENDENCIES
unexport KCONFIG_SPLITCONFIG
unexport KCONFIG_TRISTATE
unexport KCONFIG_NEGATIVES

all: tint

tint: patch
	echo "    MAKE       TINT     "
	$(MAKE) -C tint

patch: download
	cd tint; \
	if [ -e debian ]; then patch -l -p1 < ../libpayload_tint.patch; fi

download:
	test -d tint || { wget $(project_url); \
		tar -xvf $(archive_name); \
		rm $(archive_name); \
		mv tint-0.03b tint; }

clean:
	test -d tint && $(MAKE) -C tint clean || exit 0

distclean:
	rm -rf tint

.PHONY: download patch tint clean distclean

project_url=http://http.debian.net/debian/pool/main/t/tint/tint_0.04+nmu1.tar.gz
archive_name=tint_0.04+nmu1.tar.gz

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
	if [ -e debian ]; then \
		rm -rf debian typedefs.h Makefile; \
    		touch Makefile; \
    		patch -l -p1 < ../libpayload_tint.patch; \
  	fi

download:
	test -d tint || { wget $(project_url); \
		tar -xvf $(archive_name); \
		rm $(archive_name); \
		mv tint-0.04+nmu1 tint; }

clean:
	test -d tint && $(MAKE) -C tint clean || exit 0

distclean:
	rm -rf tint

.PHONY: download patch tint clean distclean

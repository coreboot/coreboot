project_url=https://github.com/pcengines/sgabios/releases/download/v1.0.0/sgabios.bin

unexport KCONFIG_AUTOHEADER
unexport KCONFIG_AUTOCONFIG
unexport KCONFIG_DEPENDENCIES
unexport KCONFIG_SPLITCONFIG
unexport KCONFIG_TRISTATE
unexport KCONFIG_NEGATIVES

all: sgabios

sgabios:
	echo "    DOWNLOAD       sgabios     "
	test -f sgabios.bin || wget $(project_url)

clean:

distclean:
	rm -rf sgabios.bin

.PHONY: download patch sgabios clean distclean

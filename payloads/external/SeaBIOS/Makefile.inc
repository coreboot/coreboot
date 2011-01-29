

TAG-$(CONFIG_SEABIOS_MASTER)=origin/master
TAG-$(CONFIG_SEABIOS_STABLE)=rel-0.6.1.3

all: seabios

seabios: patch
	cd seabios; $(MAKE)

patch: checkout
	test -r seabios/.patched || \
	perl -pi -e "s,#define CONFIG_COREBOOT 0,#define CONFIG_COREBOOT 1,;" \
		 -e "s,#define CONFIG_DEBUG_SERIAL 0,#define CONFIG_DEBUG_SERIAL 1,;" \
		 -e "s,#define CONFIG_VGAHOOKS 0,#define CONFIG_VGAHOOKS 1,;" \
		 seabios/src/config.h
	touch seabios/.patched

checkout:
	echo "Checking out SeaBIOS $(TAG-y)"
	test -d seabios && ( cd seabios; git fetch ) || \
	git clone git://git.linuxtogo.org/home/kevin/seabios.git seabios
	cd seabios; git checkout -m $(TAG-y)

clean:
	test -d seabios && (cd seabios; $(MAKE) clean) || exit 0

distclean:
	rm -rf seabios

.PHONY: seabios

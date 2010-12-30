

TAG-$(CONFIG_SEABIOS_MASTER)=master
TAG-$(CONFIG_SEABIOS_STABLE)=0.6.1-stable

all: seabios

seabios: patch
	cd seabios; $(MAKE) CC="$(CC)" LD="$(LD)"

patch: checkout
	test -r seabios/.patched || \
	perl -pi -e "s,#define CONFIG_COREBOOT 0,#define CONFIG_COREBOOT 1,;" \
		 -e "s,#define CONFIG_DEBUG_SERIAL 0,#define CONFIG_DEBUG_SERIAL 1,;" \
		 -e "s,#define CONFIG_VGAHOOKS 0,#define CONFIG_VGAHOOKS 1,;" \
		 seabios/src/config.h
	touch seabios/.patched

checkout:
	echo "Checking out SeaBIOS $(TAG-y)"
	test -d seabios && ( cd seabios; git pull ) || \
	git clone git://git.linuxtogo.org/home/kevin/seabios.git seabios
	cd seabios; git checkout $(TAG-y)

clean:
	test -d seabios && (cd seabios; $(MAKE) CC="$(CC)" LD="$(LD)" clean) || exit 0

distclean:
	rm -rf seabios

.PHONY: seabios

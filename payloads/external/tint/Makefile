#
# TINT build system - helps to securely download TINT with a checksum verification and build it.
#

#
# Properties of the current TINT version
#

TINT_VERSION=0.07
TINT_BASE_URL="https://mirror.fsf.org/trisquel/pool/main/t/tint"
TINT_ARCHIVE="tint_${TINT_VERSION}.tar.xz"
TINT_DIR="tint-${TINT_VERSION}"
TINT_SHA1SUM="a7ec9355b9ea9d47576757219e9b1f4e51ac93a7"

#
# Locations of the input/output scripts
#

buildgcc="./../../../util/crossgcc/buildgcc"
corescript="./core.sh"
tintified="./tint.sh"

unexport KCONFIG_AUTOHEADER
unexport KCONFIG_AUTOCONFIG
unexport KCONFIG_DEPENDENCIES
unexport KCONFIG_SPLITCONFIG
unexport KCONFIG_TRISTATE
unexport KCONFIG_NEGATIVES

all: tint

################################################################################
#
# Three stages of TINT build system:
#
# 1) generate_core.sh extracts the core part from buildgcc script,
#     most importantly the checksum calculation/verification functions.
#
# 2) tintify_core.sh adds the TINT-specific footer/header to the core,
#     such as the properties of current version including its checksum.
#
# 3) tint.sh - generated and "tintified" core script - builds a TINT.
#
################################################################################

tint:
	if [ ! -f ${tintified} ]; then \
		chmod +x "./generate_core.sh" ; \
		"./generate_core.sh" ${buildgcc} ${corescript} "prepare_before_patch" ; \
		chmod +x "./tintify_core.sh" ; \
		"./tintify_core.sh" ${corescript} ${tintified} \
			${TINT_BASE_URL} ${TINT_ARCHIVE} ${TINT_DIR} ${TINT_SHA1SUM} ; \
	fi ; \
	chmod +x ${tintified}
	${tintified}

clean:
	test -d "./tint/" && $(MAKE) -C "./tint/" clean || exit 0

distclean:
	rm -rf "./tint/"
	rm -f ${corescript}
	rm -f ${tintified}

.PHONY: tint clean distclean

#

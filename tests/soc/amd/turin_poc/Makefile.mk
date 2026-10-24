# SPDX-License-Identifier: GPL-2.0-only

turin_fabric_opensil ?= src/vendorcode/amd/opensil/turin_poc/opensil

ifneq ($(wildcard $(turin_fabric_opensil)/Include/xPrfFabric.h),)
tests-y += turin-fabric-1p-test turin-fabric-2p-test

turin-fabric-1p-test-srcs += tests/soc/amd/turin_poc/fabric-test.c
turin-fabric-1p-test-srcs += src/soc/amd/turin_poc/fabric.c
turin-fabric-1p-test-srcs += src/soc/amd/common/block/root_complex/root_complex.c
turin-fabric-1p-test-srcs += src/device/device_util.c tests/stubs/die.c
turin-fabric-1p-test-cflags += -Isrc/soc/amd/turin_poc/include
turin-fabric-1p-test-cflags += -Isrc/soc/amd/common/block/include
turin-fabric-1p-test-cflags += -I$(turin_fabric_opensil)/Include
turin-fabric-1p-test-cflags += -D_MSC_EXTENSIONS=0 -Wno-unknown-pragmas
turin-fabric-1p-test-config += CONFIG_MAX_SOCKET=1 CONFIG_ECAM_MMCONF_BUS_NUMBER=256
turin-fabric-1p-test-config += CONFIG_ECAM_MMCONF_LENGTH=0x10000000

$(call copy-test,turin-fabric-1p-test,turin-fabric-2p-test)
turin-fabric-2p-test-config := CONFIG_MAX_SOCKET=2 CONFIG_ECAM_MMCONF_BUS_NUMBER=512
turin-fabric-2p-test-config += CONFIG_ECAM_MMCONF_LENGTH=0x20000000

tests-y += turin-fabric-2p-single-segment-test
$(call copy-test,turin-fabric-2p-test,turin-fabric-2p-single-segment-test)
turin-fabric-2p-single-segment-test-config := CONFIG_MAX_SOCKET=2
turin-fabric-2p-single-segment-test-config += CONFIG_ECAM_MMCONF_BUS_NUMBER=256
turin-fabric-2p-single-segment-test-config += CONFIG_ECAM_MMCONF_LENGTH=0x10000000
endif

tests-y += turin-io-test turin-smm-io-test

turin-io-test-srcs += tests/soc/amd/turin_poc/io-test.c
turin-io-test-srcs += src/soc/amd/common/block/smn/smn.c
turin-io-test-srcs += src/soc/amd/common/block/root_complex/root_complex.c
turin-io-test-srcs += src/soc/amd/common/block/root_complex/non_pci_resources.c
turin-io-test-srcs += src/soc/amd/common/block/psp/psp_gen2_mmio.c
turin-io-test-srcs += src/soc/amd/turin_poc/root_complex.c
turin-io-test-srcs += src/soc/amd/turin_poc/domain.c
turin-io-test-srcs += src/device/device_util.c tests/stubs/die.c
turin-io-test-cflags += -Isrc/soc/amd/turin_poc/include
turin-io-test-cflags += -Isrc/soc/amd/common/block/include
turin-io-test-cflags += -include tests/soc/amd/turin_poc/include/mock-pci.h
turin-io-test-config += CONFIG_MAX_SOCKET=2 CONFIG_ECAM_MMCONF_BUS_NUMBER=512
turin-io-test-config += CONFIG_ECAM_MMCONF_LENGTH=0x20000000
turin-io-test-mocks += resource_range_idx

$(call copy-test,turin-io-test,turin-smm-io-test)
turin-smm-io-test-stage := smm
turin-smm-io-test-srcs := $(filter-out src/soc/amd/turin_poc/domain.c \
	src/device/device_util.c \
	src/soc/amd/common/block/root_complex/root_complex.c \
	src/soc/amd/common/block/root_complex/non_pci_resources.c,$(turin-smm-io-test-srcs))


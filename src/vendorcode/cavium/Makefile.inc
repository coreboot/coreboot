##
## This file is part of the coreboot project.
##
## Copyright 2017-present Facebook, Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

ifeq ($(CONFIG_CAVIUM_BDK),y)

romstage-y += bdk/libbdk-arch/bdk-csr.c
romstage-y += bdk/libbdk-arch/bdk-model.c
romstage-y += bdk/libbdk-arch/bdk-numa.c
romstage-y += bdk/libbdk-boot/bdk-boot-status.c
romstage-y += bdk/libbdk-dram/bdk-dram-address.c
romstage-y += bdk/libbdk-dram/bdk-dram-config.c
romstage-y += bdk/libbdk-dram/bdk-dram-size.c
romstage-y += bdk/libbdk-dram/bdk-dram-test.c
romstage-y += bdk/libbdk-dram/bdk-dram-test-addrbus.c
romstage-y += bdk/libbdk-dram/bdk-dram-test-databus.c
romstage-y += bdk/libbdk-dram/bdk-dram-test-fastscan.c
romstage-y += bdk/libbdk-dram/bdk-dram-test-patfil.c
romstage-y += bdk/libbdk-driver/bdk-driver-rnm.c
romstage-y += bdk/libbdk-hal/bdk-clock.c
romstage-y += bdk/libbdk-hal/bdk-config.c
romstage-y += bdk/libbdk-hal/bdk-gpio.c
romstage-y += bdk/libbdk-hal/bdk-l2c.c
romstage-y += bdk/libbdk-os/bdk-init.c
romstage-y += bdk/libbdk-trust/bdk-trust.c
romstage-y += bdk/libdram/dram-env.c
romstage-y += bdk/libdram/dram-init-ddr3.c
romstage-y += bdk/libdram/dram-l2c.c
romstage-y += bdk/libdram/dram-spd.c
romstage-y += bdk/libdram/dram-tune-ddr3.c
romstage-y += bdk/libdram/lib_octeon_shared.c
romstage-y += bdk/libdram/libdram.c
romstage-y += bdk/libdram/libdram-config-load.c
romstage-y += bdk/libbdk-hal/bdk-access.c

# FIXME: Get rid of lame_string.c
romstage-y += bdk/lame_string.c

CPPFLAGS_common += -Isrc/vendorcode/cavium/include/bdk

# For bdk_dram_get_size_mbytes()
ramstage-y += bdk/libbdk-dram/bdk-dram-size.c

ramstage-y += bdk/libbdk-hal/bdk-config.c
ramstage-y += bdk/libbdk-hal/bdk-qlm.c
ramstage-y += bdk/libbdk-hal/bdk-pcie-cn8xxx.c
ramstage-y += bdk/libbdk-hal/bdk-pcie.c
ramstage-y += bdk/libbdk-hal/bdk-gpio.c
ramstage-y += bdk/libbdk-hal/bdk-ecam-io.c
ramstage-y += bdk/libbdk-hal/bdk-usb.c
ramstage-y += bdk/libbdk-hal/bdk-access.c

ramstage-y += bdk/libbdk-arch/bdk-csr.c
ramstage-y += bdk/libbdk-arch/bdk-model.c
ramstage-y += bdk/libbdk-arch/bdk-numa.c
ramstage-y += bdk/libbdk-hal/qlm/bdk-qlm-common.c
ramstage-y += bdk/libbdk-hal/qlm/bdk-qlm-errata-cn8xxx.c
ramstage-y += bdk/libbdk-hal/qlm/bdk-qlm-common-sata.c
ramstage-y += bdk/libbdk-hal/qlm/bdk-qlm-margin-cn8xxx.c

ramstage-y += bdk/libbdk-boot/bdk-boot-qlm.c
ramstage-y += bdk/libbdk-boot/bdk-boot-pcie.c
ramstage-y += bdk/libbdk-boot/bdk-boot-usb.c
ramstage-y += bdk/libbdk-boot/bdk-boot-gpio.c
ramstage-y += bdk/libbdk-boot/bdk-boot.c

ramstage-y += bdk/libbdk-hal/if/bdk-if-phy.c
ramstage-y += bdk/libbdk-hal/if/bdk-if-phy-marvell.c
ramstage-y += bdk/libbdk-hal/if/bdk-if-phy-vetesse-8514.c
ramstage-y += bdk/libbdk-hal/if/bdk-if-phy-vetesse.c
ramstage-y += bdk/libbdk-driver/bdk-driver-mdio.c
ramstage-y += bdk/libbdk-driver/bdk-driver-rnm.c
ramstage-y += bdk/libbdk-hal/device/bdk-device.c
ramstage-y += bdk/libbdk-hal/bdk-ecam.c

# FIXME: Get rid of lame_string.c
ramstage-y += bdk/lame_string.c

ramstage-$(CONFIG_SOC_CAVIUM_CN81XX) += bdk/libbdk-hal/qlm/bdk-qlm-cn81xx.c

endif

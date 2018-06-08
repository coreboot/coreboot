##
## This file is part of the coreboot project.
##
## Copyright 2015 Google Inc.
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

ifeq ($(CONFIG_SOC_BROADCOM_CYGNUS),y)

bootblock-y += bootblock.c
bootblock-y += cbmem.c
bootblock-y += i2c.c
bootblock-y += timer.c
bootblock-y += tz.c
bootblock-y += hw_init.c
bootblock-$(CONFIG_SPI_FLASH) += spi.c
ifeq ($(CONFIG_BOOTBLOCK_CONSOLE),y)
bootblock-$(CONFIG_DRIVERS_UART) += ns16550.c
endif

verstage-y += i2c.c
verstage-y += timer.c
verstage-$(CONFIG_SPI_FLASH) += spi.c
verstage-$(CONFIG_DRIVERS_UART) += ns16550.c

romstage-y += cbmem.c
romstage-y += i2c.c
romstage-y += timer.c
romstage-y += romstage.c
romstage-y += sdram.c
romstage-$(CONFIG_SPI_FLASH) += spi.c
romstage-$(CONFIG_DRIVERS_UART) += ns16550.c
romstage-y += ddr_init.c
romstage-y += ddr_init_table.c
romstage-y += shmoo_and28.c
romstage-y += phy_reg_access.c
romstage-y += ydc_ddr_bist.c
romstage-y += timer.c
romstage-y += gpio.c
romstage-y += iomux.c

ramstage-y += cbmem.c
ramstage-y += i2c.c
ramstage-y += sdram.c
ramstage-y += soc.c
ramstage-y += timer.c
ramstage-$(CONFIG_SPI_FLASH) += spi.c
ramstage-$(CONFIG_DRIVERS_UART) += ns16550.c
ramstage-y += usb.c

CPPFLAGS_common += -Isrc/soc/broadcom/cygnus/include/

ifneq ($(V),1)
redirect := > /dev/null
endif

# Options used in the command line:
# -out: path of the output file
# -config: path to the file containing unauth header
# -hmac: path to the file containing hmac for sha256
# -bl: boot image file, ie. input file
#
# Authenticated header parameters:
#
# SBIConfiguration				/* Indicates SBI config */
#   SYMMETRIC				0x0040
#
# CustomerID;			/* Customer ID */
#   TYPE				bits [31-28]
#     PRODUCTION			0x6
#     DEVELOPMENT			0x9
#   CUSTOMER_ID				bits [27-0]
#
# ProductID;			/* Product ID */
#
# CustomerRevisionID;		/* Customer Revision ID */
#
# SBIUsage			/* Boot Image Usage */
#   NONE		0	/* All purposes */
#   SLEEP		1
#   DEEP_SLEEP		2
#   EXCEPTION		4
$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin \
		$(objutil)/broadcom/secimage/secimage \
		util/broadcom/unauth.cfg \
		util/broadcom/khmacsha256
	@printf "    SIGN       $(subst $(obj)/,,$(@))\n"
	$(objutil)/broadcom/secimage/secimage -out $@ \
		-config util/broadcom/unauth.cfg \
		-hmac util/broadcom/khmacsha256 -bl $<

endif

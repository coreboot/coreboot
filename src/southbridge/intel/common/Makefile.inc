##
## This file is part of the coreboot project.
##
## Copyright (C) 2008-2009 coresystems GmbH
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

# CONFIG_HAVE_INTEL_FIRMWARE protects doing anything to the build.
subdirs-y += firmware

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_COMMON),y)

romstage-y += pmbase.c
ramstage-y += pmbase.c
postcar-y += pmbase.c
smm-y += pmbase.c

romstage-$(CONFIG_USBDEBUG_IN_ROMSTAGE) += usb_debug.c
ramstage-$(CONFIG_USBDEBUG) += usb_debug.c

romstage-$(CONFIG_SOUTHBRIDGE_INTEL_COMMON_GPIO) += gpio.c
ramstage-$(CONFIG_SOUTHBRIDGE_INTEL_COMMON_GPIO) += gpio.c
smm-$(CONFIG_SOUTHBRIDGE_INTEL_COMMON_GPIO) += gpio.c

romstage-$(CONFIG_SOUTHBRIDGE_INTEL_COMMON_SMBUS) += smbus.c
ramstage-$(CONFIG_SOUTHBRIDGE_INTEL_COMMON_SMBUS) += smbus.c

romstage-$(CONFIG_SOUTHBRIDGE_INTEL_COMMON_SPI) += spi.c
postcar-$(CONFIG_SOUTHBRIDGE_INTEL_COMMON_SPI) += spi.c
ramstage-$(CONFIG_SOUTHBRIDGE_INTEL_COMMON_SPI) += spi.c
ifeq ($(CONFIG_SPI_FLASH_SMM),y)
smm-$(CONFIG_SOUTHBRIDGE_INTEL_COMMON_SPI) += spi.c
endif

ramstage-$(CONFIG_SOUTHBRIDGE_INTEL_COMMON_PIRQ_ACPI_GEN) += acpi_pirq_gen.c
ramstage-$(CONFIG_SOUTHBRIDGE_INTEL_COMMON_RCBA_PIRQ) += rcba_pirq.c

ramstage-$(CONFIG_SOUTHBRIDGE_INTEL_COMMON_SMM) += pmutil.c smi.c
smm-$(CONFIG_SOUTHBRIDGE_INTEL_COMMON_SMM) += pmutil.c smihandler.c

endif

## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_INTEL_COMMON_BLOCK_I2C),y)

bootblock-y += i2c.c
romstage-y += i2c.c
verstage-y += i2c.c
postcar-y += i2c.c
ramstage-y += i2c.c

endif

## SPDX-License-Identifier: GPL-2.0-only

verstage-$(CONFIG_DRIVERS_I2C_WW_RING) += ww_ring.c
verstage-$(CONFIG_DRIVERS_I2C_WW_RING) += ww_ring_programs.c

ramstage-$(CONFIG_DRIVERS_I2C_WW_RING) += ww_ring.c
ramstage-$(CONFIG_DRIVERS_I2C_WW_RING) += ww_ring_programs.c

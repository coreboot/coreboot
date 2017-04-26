bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_I2C) += lpss_i2c.c
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_I2C) += i2c_early.c

romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_I2C) += lpss_i2c.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_I2C) += i2c_early.c

verstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_I2C) += lpss_i2c.c
verstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_I2C) += i2c_early.c

ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_I2C) += lpss_i2c.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_I2C) += i2c.c

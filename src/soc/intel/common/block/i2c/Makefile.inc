ifeq ($(CONFIG_SOC_INTEL_COMMON_BLOCK_I2C),y)

bootblock-y += i2c_early.c
romstage-y += i2c_early.c
verstage-y += i2c_early.c
ramstage-y += i2c.c

endif

ramstage-$(CONFIG_DRIVERS_I2C_PCA9538) += pca9538.c

ifeq ($(CONFIG_DRIVERS_I2C_PCA9538),y)
CFLAGS_common += -Isrc/drivers/i2c/pca9538
endif

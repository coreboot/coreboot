ifeq ($(CONFIG_DRIVERS_I2C_DESIGNWARE),y)

bootblock-y += dw_i2c.c
romstage-y += dw_i2c.c
verstage-y += dw_i2c.c
ramstage-y += dw_i2c.c
postcar-y += dw_i2c.c

endif

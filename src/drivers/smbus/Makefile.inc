ifeq ($(CONFIG_SC16IS7XX_INIT),y)
bootblock-y += sc16is7xx_init.c
romstage-y += sc16is7xx_init.c
endif

ifeq ($(CONFIG_CONSOLE_I2C_SMBUS),y)
all-y += i2c_smbus_console.c
endif

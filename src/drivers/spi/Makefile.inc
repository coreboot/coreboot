# SPI driver interface
ramstage-$(CONFIG_SPI) += spi_flash.c

# drivers
ramstage-$(CONFIG_SPI) += eon.c
ramstage-$(CONFIG_SPI) += macronix.c
ramstage-$(CONFIG_SPI) += ramtron.c
ramstage-$(CONFIG_SPI) += spansion.c
ramstage-$(CONFIG_SPI) += sst.c
ramstage-$(CONFIG_SPI) += stmicro.c
ramstage-$(CONFIG_SPI) += winbond.c


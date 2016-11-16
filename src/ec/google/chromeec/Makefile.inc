ifeq ($(CONFIG_EC_GOOGLE_CHROMEEC),y)

bootblock-y += ec.c
bootblock-$(CONFIG_EC_GOOGLE_CHROMEEC_LPC) += ec_lpc.c
bootblock-$(CONFIG_EC_GOOGLE_CHROMEEC_MEC) += ec_mec.c
ramstage-y += ec.c crosec_proto.c vstore.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC_I2C) += ec_i2c.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC_LPC) += ec_lpc.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC_MEC) += ec_mec.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC_SPI) += ec_spi.c
smm-y += ec.c crosec_proto.c smihandler.c vstore.c
smm-$(CONFIG_EC_GOOGLE_CHROMEEC_I2C) += ec_i2c.c
smm-$(CONFIG_EC_GOOGLE_CHROMEEC_LPC) += ec_lpc.c
smm-$(CONFIG_EC_GOOGLE_CHROMEEC_MEC) += ec_mec.c
smm-$(CONFIG_EC_GOOGLE_CHROMEEC_SPI) += ec_spi.c
romstage-y += ec.c crosec_proto.c vstore.c
romstage-$(CONFIG_EC_GOOGLE_CHROMEEC_I2C) += ec_i2c.c
romstage-$(CONFIG_EC_GOOGLE_CHROMEEC_LPC) += ec_lpc.c
romstage-$(CONFIG_EC_GOOGLE_CHROMEEC_MEC) += ec_mec.c
romstage-$(CONFIG_EC_GOOGLE_CHROMEEC_SPI) += ec_spi.c
verstage-y += ec.c crosec_proto.c vstore.c
verstage-$(CONFIG_EC_GOOGLE_CHROMEEC_I2C) += ec_i2c.c
verstage-$(CONFIG_EC_GOOGLE_CHROMEEC_LPC) += ec_lpc.c
verstage-$(CONFIG_EC_GOOGLE_CHROMEEC_MEC) += ec_mec.c
verstage-$(CONFIG_EC_GOOGLE_CHROMEEC_SPI) += ec_spi.c

ramstage-$(CONFIG_VBOOT) += vboot_storage.c
smm-$(CONFIG_VBOOT) += vboot_storage.c
romstage-$(CONFIG_VBOOT) += vboot_storage.c
verstage-$(CONFIG_VBOOT) += vboot_storage.c

verstage-$(CONFIG_EC_GOOGLE_CHROMEEC_SWITCHES) += switches.c
romstage-$(CONFIG_EC_GOOGLE_CHROMEEC_SWITCHES) += switches.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC_SWITCHES) += switches.c

ifneq ($(CONFIG_EC_EXTERNAL_FIRMWARE),y)
# These are Chrome EC firmware images that a payload (such as depthcharge) can
# use to update the EC. ecrw is the main embedded controller's firmware,
# pdrw is for a USB PD controller.
CONFIG_EC_GOOGLE_CHROMEEC_BOARDNAME := $(call strip_quotes,$(CONFIG_EC_GOOGLE_CHROMEEC_BOARDNAME))
CONFIG_EC_GOOGLE_CHROMEEC_PD_BOARDNAME := $(call strip_quotes,$(CONFIG_EC_GOOGLE_CHROMEEC_PD_BOARDNAME))

cbfs-files-$(if $(CONFIG_EC_GOOGLE_CHROMEEC_BOARDNAME),y) += ecrw
ecrw-file := $(obj)/mainboard/$(MAINBOARDDIR)/ecrw
ecrw-name := ecrw
ecrw-type := raw
ecrw-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-$(if $(CONFIG_EC_GOOGLE_CHROMEEC_BOARDNAME),y) += ecrw.hash
ecrw.hash-file := $(obj)/mainboard/$(MAINBOARDDIR)/ecrw.hash
ecrw.hash-name := ecrw.hash
ecrw.hash-type := raw

cbfs-files-$(if $(CONFIG_EC_GOOGLE_CHROMEEC_PD_BOARDNAME),y) += pdrw
pdrw-file := $(obj)/mainboard/$(MAINBOARDDIR)/pdrw
pdrw-name := pdrw
pdrw-type := raw
pdrw-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-$(if $(CONFIG_EC_GOOGLE_CHROMEEC_PD_BOARDNAME),y) += pdrw.hash
pdrw.hash-file := $(obj)/mainboard/$(MAINBOARDDIR)/pdrw.hash
pdrw.hash-name := pdrw.hash
pdrw.hash-type := raw

CHROMEEC_SOURCE ?= $(top)/3rdparty/chromeec

$(obj)/mainboard/$(MAINBOARDDIR)/ecrw:
	$(MAKE) -C $(CHROMEEC_SOURCE) $(if $(CONFIG_CCACHE),,CCACHE=) \
		out=$(abspath $(obj)/external/chromeec/$(CONFIG_EC_GOOGLE_CHROMEEC_BOARDNAME)) \
		CROSS_COMPILE=$(subst -cpp,-,$(CPP_arm)) \
		HOST_CROSS_COMPILE= \
		BOARD=$(CONFIG_EC_GOOGLE_CHROMEEC_BOARDNAME) \
		rw
	cp $(obj)/external/chromeec/$(CONFIG_EC_GOOGLE_CHROMEEC_BOARDNAME)/RW/ec.RW.flat $@

$(obj)/mainboard/$(MAINBOARDDIR)/pdrw:
	$(MAKE) -C $(CHROMEEC_SOURCE) $(if $(CONFIG_CCACHE),,CCACHE=) \
		out=$(abspath $(obj)/external/chromeec/$(CONFIG_EC_GOOGLE_CHROMEEC_PD_BOARDNAME)) \
		CROSS_COMPILE=$(subst -cpp,-,$(CPP_arm)) \
		HOST_CROSS_COMPILE= \
		BOARD=$(CONFIG_EC_GOOGLE_CHROMEEC_PD_BOARDNAME) \
		rw
	cp $(obj)/external/chromeec/$(CONFIG_EC_GOOGLE_CHROMEEC_PD_BOARDNAME)/RW/ec.RW.flat $@

$(obj)/mainboard/$(MAINBOARDDIR)/%.hash: $(obj)/mainboard/$(MAINBOARDDIR)/%
	openssl dgst -sha256 -binary $< > $@

.PHONY: $(obj)/mainboard/$(MAINBOARDDIR)/ecrw $(obj)/mainboard/$(MAINBOARDDIR)/pdrw
endif

endif

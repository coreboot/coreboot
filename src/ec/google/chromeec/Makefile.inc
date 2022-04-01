ifeq ($(CONFIG_EC_GOOGLE_CHROMEEC),y)

subdirs-y += audio_codec
subdirs-y += i2c_tunnel
subdirs-y += mux

bootblock-$(CONFIG_EC_GOOGLE_CHROMEEC_BOARDID) += ec_boardid.c
verstage-$(CONFIG_EC_GOOGLE_CHROMEEC_BOARDID) += ec_boardid.c
romstage-$(CONFIG_EC_GOOGLE_CHROMEEC_BOARDID) += ec_boardid.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC_BOARDID) += ec_boardid.c
smm-$(CONFIG_EC_GOOGLE_CHROMEEC_BOARDID) += ec_boardid.c

smm-$(CONFIG_EC_GOOGLE_CHROMEEC_SKUID) += ec_skuid.c
romstage-$(CONFIG_EC_GOOGLE_CHROMEEC_SKUID) += ec_skuid.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC_SKUID) += ec_skuid.c

ifeq ($(CONFIG_GENERATE_SMBIOS_TABLES),y)
romstage-$(CONFIG_EC_GOOGLE_CHROMEEC_SKUID) += ec_smbios.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC_SKUID) += ec_smbios.c
endif

bootblock-y += ec.c
bootblock-$(CONFIG_EC_GOOGLE_CHROMEEC_LPC) += ec_lpc.c
ramstage-y += ec.c crosec_proto.c vstore.c usbc_mux.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC_I2C) += ec_i2c.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC_LPC) += ec_lpc.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC_SPI) += ec_spi.c
smm-y += ec.c crosec_proto.c smihandler.c vstore.c
smm-$(CONFIG_EC_GOOGLE_CHROMEEC_I2C) += ec_i2c.c
smm-$(CONFIG_EC_GOOGLE_CHROMEEC_LPC) += ec_lpc.c
smm-$(CONFIG_EC_GOOGLE_CHROMEEC_SPI) += ec_spi.c
romstage-y += ec.c crosec_proto.c vstore.c
romstage-$(CONFIG_EC_GOOGLE_CHROMEEC_I2C) += ec_i2c.c
romstage-$(CONFIG_EC_GOOGLE_CHROMEEC_LPC) += ec_lpc.c
romstage-$(CONFIG_EC_GOOGLE_CHROMEEC_SPI) += ec_spi.c
verstage-y += ec.c crosec_proto.c vstore.c
verstage-$(CONFIG_EC_GOOGLE_CHROMEEC_I2C) += ec_i2c.c
verstage-$(CONFIG_EC_GOOGLE_CHROMEEC_LPC) += ec_lpc.c
verstage-$(CONFIG_EC_GOOGLE_CHROMEEC_SPI) += ec_spi.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += ec_acpi.c

ramstage-$(CONFIG_VBOOT) += vboot_storage.c
smm-$(CONFIG_VBOOT) += vboot_storage.c
romstage-$(CONFIG_VBOOT) += vboot_storage.c
verstage-$(CONFIG_VBOOT) += vboot_storage.c

verstage-$(CONFIG_EC_GOOGLE_CHROMEEC_SWITCHES) += switches.c
romstage-$(CONFIG_EC_GOOGLE_CHROMEEC_SWITCHES) += switches.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC_SWITCHES) += switches.c

ramstage-$(CONFIG_DRIVERS_INTEL_DPTF) += ec_dptf_helpers.c

CHROMEEC_SOURCE ?= $(top)/3rdparty/chromeec

# These are Chrome EC firmware images that a payload (such as depthcharge) can
# use to update the EC. ecrw is the main embedded controller's firmware,
# pdrw is for a USB PD controller.

ifneq ($(CONFIG_EC_GOOGLE_CHROMEEC_FIRMWARE_NONE),y)

cbfs-files-y += ecrw
ecrw-file := $(obj)/mainboard/$(MAINBOARDDIR)/ecrw
ecrw-name := ecrw
ecrw-type := raw
ecrw-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += ecrw.hash
ecrw.hash-file := $(obj)/mainboard/$(MAINBOARDDIR)/ecrw.hash
ecrw.hash-name := ecrw.hash
ecrw.hash-type := raw

ifeq ($(CONFIG_EC_GOOGLE_CHROMEEC_FIRMWARE_EXTERNAL),y)
CONFIG_EC_GOOGLE_CHROMEEC_FIRMWARE_FILE := $(call strip_quotes,$(CONFIG_EC_GOOGLE_CHROMEEC_FIRMWARE_FILE))

$(obj)/mainboard/$(MAINBOARDDIR)/ecrw: $(CONFIG_EC_GOOGLE_CHROMEEC_FIRMWARE_FILE)
	cp $(CONFIG_EC_GOOGLE_CHROMEEC_FIRMWARE_FILE) $@
else
CONFIG_EC_GOOGLE_CHROMEEC_BOARDNAME := $(call strip_quotes,$(CONFIG_EC_GOOGLE_CHROMEEC_BOARDNAME))

$(obj)/mainboard/$(MAINBOARDDIR)/ecrw:
	$(MAKE) -C $(CHROMEEC_SOURCE) $(if $(CONFIG_CCACHE),,CCACHE=) \
		out=$(abspath $(obj)/external/chromeec/$(CONFIG_EC_GOOGLE_CHROMEEC_BOARDNAME)) \
		REPRODUCIBLE_BUILD=1 \
		CC=$(GCC_CC_arm) \
		CROSS_COMPILE=$(subst -cpp,-,$(CPP_arm)) \
		HOST_CROSS_COMPILE= \
		BOARD=$(CONFIG_EC_GOOGLE_CHROMEEC_BOARDNAME) \
		rw
	cp $(obj)/external/chromeec/$(CONFIG_EC_GOOGLE_CHROMEEC_BOARDNAME)/RW/ec.RW.flat $@
endif

$(obj)/mainboard/$(MAINBOARDDIR)/ecrw.hash: $(obj)/mainboard/$(MAINBOARDDIR)/ecrw
	openssl dgst -sha256 -binary $< > $@

endif

ifeq ($(CONFIG_EC_GOOGLE_CHROMEEC_PD),y)

ifneq ($(CONFIG_EC_GOOGLE_CHROMEEC_PD_FIRMWARE_NONE),y)

cbfs-files-y += pdrw
pdrw-file := $(obj)/mainboard/$(MAINBOARDDIR)/pdrw
pdrw-name := pdrw
pdrw-type := raw
pdrw-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += pdrw.hash
pdrw.hash-file := $(obj)/mainboard/$(MAINBOARDDIR)/pdrw.hash
pdrw.hash-name := pdrw.hash
pdrw.hash-type := raw

ifeq ($(CONFIG_EC_GOOGLE_CHROMEEC_PD_FIRMWARE_EXTERNAL),y)
CONFIG_EC_GOOGLE_CHROMEEC_PD_FIRMWARE_FILE := $(call strip_quotes,$(CONFIG_EC_GOOGLE_CHROMEEC_PD_FIRMWARE_FILE))

$(obj)/mainboard/$(MAINBOARDDIR)/pdrw: $(CONFIG_EC_GOOGLE_CHROMEEC_PD_FIRMWARE_FILE)
	cp $(CONFIG_EC_GOOGLE_CHROMEEC_PD_FIRMWARE_FILE) $@
else
CONFIG_EC_GOOGLE_CHROMEEC_PD_BOARDNAME := $(call strip_quotes,$(CONFIG_EC_GOOGLE_CHROMEEC_PD_BOARDNAME))

$(obj)/mainboard/$(MAINBOARDDIR)/pdrw:
	$(MAKE) -C $(CHROMEEC_SOURCE) $(if $(CONFIG_CCACHE),,CCACHE=) \
		out=$(abspath $(obj)/external/chromeec/$(CONFIG_EC_GOOGLE_CHROMEEC_PD_BOARDNAME)) \
		REPRODUCIBLE_BUILD=1 \
		CC=$(GCC_CC_arm) \
		CROSS_COMPILE=$(subst -cpp,-,$(CPP_arm)) \
		HOST_CROSS_COMPILE= \
		BOARD=$(CONFIG_EC_GOOGLE_CHROMEEC_PD_BOARDNAME) \
		rw
	cp $(obj)/external/chromeec/$(CONFIG_EC_GOOGLE_CHROMEEC_PD_BOARDNAME)/RW/ec.RW.flat $@
endif

$(obj)/mainboard/$(MAINBOARDDIR)/pdrw.hash: $(obj)/mainboard/$(MAINBOARDDIR)/pdrw
	openssl dgst -sha256 -binary $< > $@

endif

endif

endif

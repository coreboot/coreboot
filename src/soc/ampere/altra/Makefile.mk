## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_AMPERE_ALTRA),y)

all-y += atf.c
all-y += media.c
all-y += uart.c

bootblock-y += bootblock_custom.S
bootblock-y += bootblock.c
bootblock-y += cbmem.c

ramstage-y += hob.c
ramstage-y += soc.c
ramstage-y += watchdog.c

AMPERE_TFA_FILE := $(call strip_quotes,$(CONFIG_AMPERE_ALTRA_TFA_FILE))
AMPERE_BL33_KEY := $(call strip_quotes,$(CONFIG_AMPERE_ALTRA_BL33_KEY))
AMPERE_BOARD_SETTINGS := $(call strip_quotes,$(CONFIG_AMPERE_ALTRA_BOARD_SETTINGS_FILE))

# The image only boots with all of these files, but it is built without the
# missing ones (with a warning at the end), so that the build bots can build it.
AMPERE_MISSING_FILES :=

ifneq ($(wildcard $(AMPERE_TFA_FILE)),)
$(call add_intermediate, add_tfa, $(AMPERE_TFA_FILE))
	$(CBFSTOOL) $(obj)/coreboot.pre write -u -r TFA_BIN -f $(AMPERE_TFA_FILE)
else
AMPERE_MISSING_FILES += 'TF-A: $(AMPERE_TFA_FILE)'
endif

ifneq ($(wildcard $(AMPERE_BOARD_SETTINGS)),)
ifeq ($(suffix $(AMPERE_BOARD_SETTINGS)),.bin)
$(obj)/board_settings.bin: $(AMPERE_BOARD_SETTINGS)
	cp $< $@
else
$(obj)/board_settings.bin: $(AMPERE_BOARD_SETTINGS) $(top)/util/ampere/nvparam.py
	python3 $(top)/util/ampere/nvparam.py -f $< -o $@
endif

$(call add_intermediate, add_board_settings, $(obj)/board_settings.bin)
	$(CBFSTOOL) $(obj)/coreboot.pre write -u -r BOARD_SETTINGS -f $(obj)/board_settings.bin
else
AMPERE_MISSING_FILES += 'Board settings: $(AMPERE_BOARD_SETTINGS)'
endif

# TF-A loads BL33 (the bootblock) from a FIP with a signed content certificate.
# Without the key, the plain bootblock ends up in the BOOTBLOCK region.
ifneq ($(wildcard $(AMPERE_BL33_KEY)),)
CERT_CREATE=$(top)/3rdparty/arm-trusted-firmware/tools/cert_create/cert_create
FIPTOOL=$(top)/3rdparty/arm-trusted-firmware/tools/fiptool/fiptool

$(CERT_CREATE): | $(top)/3rdparty/arm-trusted-firmware
	$(MAKE) -C $(top)/3rdparty/arm-trusted-firmware/tools/cert_create

$(FIPTOOL): | $(top)/3rdparty/arm-trusted-firmware
	$(MAKE) -C $(top)/3rdparty/arm-trusted-firmware/tools/fiptool

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin $(AMPERE_BL33_KEY) $(CERT_CREATE) $(FIPTOOL)
	$(CERT_CREATE) -n --ntfw-nvctr 0 --key-alg rsa \
	                  --nt-fw-key $(AMPERE_BL33_KEY) \
	                  --nt-fw-cert $(objcbfs)/nt_fw_content.crt \
	                  --nt-fw $<
	$(FIPTOOL) create --nt-fw-cert $(objcbfs)/nt_fw_content.crt \
	                  --nt-fw $< \
	                  $@
	$(FIPTOOL) info $@
else
AMPERE_MISSING_FILES += 'BL33 signing key: $(AMPERE_BL33_KEY)'
endif

ifneq ($(strip $(AMPERE_MISSING_FILES)),)
show_notices:: warn_ampere_altra_files
endif

PHONY += warn_ampere_altra_files
warn_ampere_altra_files:
	printf "\n\t** WARNING **\n"
	printf "coreboot has been built without these Ampere Altra firmware files:\n"
	printf "\t%s\n" $(AMPERE_MISSING_FILES)
	printf "Do not flash this image, it won't boot. See the documentation of the\n"
	printf "mainboard (e.g. Documentation/mainboard/adlink/alva.md) for how to get them.\n\n"

CFLAGS_common += -mcmodel=large

CPPFLAGS_common += -Isrc/soc/ampere/altra/include
CPPFLAGS_common += -mstrict-align

endif

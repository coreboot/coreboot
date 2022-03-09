## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_EC_STARLABS_ITE),y)

PHONY+=add_ite_fw
INTERMEDIATE+=add_ite_fw

EC_VARIANT_DIR := $(call strip_quotes, $(CONFIG_EC_VARIANT_DIR))
CPPFLAGS_common += -I$(src)/ec/starlabs/merlin/variants/$(EC_VARIANT_DIR)

all-y += ite.c

ifeq ($(CONFIG_EC_STARLABS_NEED_ITE_BIN),y)
ifeq ($(CONFIG_EC_STARLABS_ADD_ITE_BIN),y)
add_ite_fw: $(obj)/coreboot.pre
	$(CBFSTOOL) $(obj)/coreboot.pre write -r EC -f $(CONFIG_EC_STARLABS_ITE_BIN_PATH) -u
else
files_added:: warn_no_ite_fw

PHONY+=warn_no_ite_fw
warn_no_ite_fw:
	printf "\n\t** WARNING **\n"
	printf "coreboot has been built without the ITE EC Firmware.\n"
	printf "Do not flash this image.  Your laptop's power button\n"
	printf "may not respond when you press it.\n\n"
endif
endif
endif

ifeq ($(CONFIG_EC_STARLABS_NUVOTON),y)

EC_VARIANT_DIR := $(call strip_quotes, $(CONFIG_EC_VARIANT_DIR))
CPPFLAGS_common += -I$(src)/ec/starlabs/merlin/variants/$(EC_VARIANT_DIR)

all-y += nuvoton.c

endif

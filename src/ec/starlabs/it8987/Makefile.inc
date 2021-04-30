## SPDX-License-Identifier: GPL-2.0-only

PHONY+=add_ite_fw
INTERMEDIATE+=add_ite_fw

ifeq ($(CONFIG_EC_STARLABS_IT8987),y)
all-y += ec.c
smm-$(CONFIG_DEBUG_SMI) += ec.c
endif

ifeq ($(CONFIG_EC_STARLABS_IT8987_BIN),y)

ifeq ($(CONFIG_EC_STARLABS_IT8987_BIN_PATH),)
files_added:: warn_no_ite_fw
endif

add_ite_fw: $(obj)/coreboot.pre
	$(CBFSTOOL) $(obj)/coreboot.pre write -r EC -f $(CONFIG_EC_STARLABS_IT8987_BIN_PATH) -u
endif

PHONY+=warn_no_ite_fw
warn_no_ite_fw:
	printf "\n\t** WARNING **\n"
	printf "coreboot has been built without the IT8987 EC Firmware.\n"
	printf "Do not flash this image.  Your LabTop Mk IV's power button\n"
	printf "may not respond when you press it.\n\n"

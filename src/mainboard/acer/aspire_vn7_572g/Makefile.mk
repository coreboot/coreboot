## SPDX-License-Identifier: GPL-2.0-only

all-y += die.c

bootblock-y += bootblock.c
bootblock-y += ec.c
bootblock-y += gpio_early.c

ramstage-y += ec.c
ramstage-y += gpio.c

smm-y += die.c
smm-y += ec.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads

files_added::
ifeq ($(CONFIG_INCLUDE_EC_FIRMWARE),y)
	$(CBFSTOOL) $(obj)/coreboot.rom write -r EC -f $(CONFIG_EC_FIRMWARE_FILE) --fill-upward
endif

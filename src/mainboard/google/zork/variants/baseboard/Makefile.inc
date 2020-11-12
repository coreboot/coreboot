# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += gpio_baseboard_common.c
bootblock-y += helpers.c
bootblock-$(CONFIG_BOARD_GOOGLE_BASEBOARD_TREMBYLE) += gpio_baseboard_trembyle.c
bootblock-$(CONFIG_BOARD_GOOGLE_BASEBOARD_DALBOZ) += gpio_baseboard_dalboz.c

verstage-y += gpio_baseboard_common.c
verstage-y += helpers.c
verstage-$(CONFIG_BOARD_GOOGLE_BASEBOARD_TREMBYLE) += gpio_baseboard_trembyle.c
verstage-$(CONFIG_BOARD_GOOGLE_BASEBOARD_DALBOZ) += gpio_baseboard_dalboz.c
verstage-y += tpm_tis.c

romstage-y += gpio_baseboard_common.c
romstage-y += helpers.c
romstage-$(CONFIG_BOARD_GOOGLE_BASEBOARD_TREMBYLE) += gpio_baseboard_trembyle.c
romstage-$(CONFIG_BOARD_GOOGLE_BASEBOARD_DALBOZ) += gpio_baseboard_dalboz.c
romstage-y += tpm_tis.c

ramstage-y += gpio_baseboard_common.c
ramstage-$(CONFIG_BOARD_GOOGLE_BASEBOARD_TREMBYLE) += gpio_baseboard_trembyle.c
ramstage-$(CONFIG_BOARD_GOOGLE_BASEBOARD_TREMBYLE) += fsps_baseboard_trembyle.c
ramstage-$(CONFIG_BOARD_GOOGLE_BASEBOARD_DALBOZ) += gpio_baseboard_dalboz.c
ramstage-$(CONFIG_BOARD_GOOGLE_BASEBOARD_DALBOZ) += fsps_baseboard_dalboz.c
ramstage-y += helpers.c
ramstage-y += tpm_tis.c
ramstage-y += ramstage_common.c

smm-$(CONFIG_BOARD_GOOGLE_BASEBOARD_TREMBYLE) += gpio_baseboard_trembyle.c
smm-$(CONFIG_BOARD_GOOGLE_BASEBOARD_DALBOZ) += gpio_baseboard_dalboz.c

# Add OEM ID table
ifeq ($(CONFIG_USE_OEM_BIN),y)
cbfs-files-y += oem.bin
oem.bin-file := $(call strip_quotes,$(CONFIG_OEM_BIN_FILE))
oem.bin-type := raw
endif #($(CONFIG_USE_OEM_BIN),y)

# APCB Board ID GPIO configuration.
# These GPIOs determine which memory SPD will be used during boot.
# APCB_BOARD_ID_GPIO[0-3] = GPIO_NUMBER GPIO_IO_MUX GPIO_BANK_CTL
# GPIO_NUMBER: FCH GPIO number
# GPIO_IO_MUX: Value write to IOMUX to configure this GPIO
# GPIO_BANK_CTL: Value write to GPIOBankCtl[23:16] to configure this GPIO
# APCB_POPULATE_2ND_CHANNEL: Populates 2nd memory channel in APCB when true.
#	Trembyle based boards select 1 or 2 channels based on AGPIO84
#	Dalboz based boards only support 1 channel
ifeq ($(CONFIG_BOARD_GOOGLE_BASEBOARD_TREMBYLE),y)
APCB_BOARD_ID_GPIO0 = 121 1 0
APCB_BOARD_ID_GPIO1 = 120 1 0
APCB_BOARD_ID_GPIO2 = 131 3 0
APCB_BOARD_ID_GPIO3 = 116 1 0
APCB_POPULATE_2ND_CHANNEL = true
else ifeq ($(CONFIG_BOARD_GOOGLE_BASEBOARD_DALBOZ),y)
APCB_BOARD_ID_GPIO0 = 132 1 0
APCB_BOARD_ID_GPIO1 = 90 1 0
APCB_BOARD_ID_GPIO2 = 86 3 0
APCB_BOARD_ID_GPIO3 = 69 1 0
else
$(error Undefined APCB selection GPIOS for Zork baseboard)
endif #($(CONFIG_BOARD_GOOGLE_BASEBOARD_TREMBYLE),y)

# SPDX-License-Identifier: GPL-2.0-or-later

all-y += gpio.c
smm-y += gpio.c
ramstage-y += fsps.c

# APCB Board ID GPIO configuration.
# These GPIOs determine which memory SPD will be used during boot.
# APCB_BOARD_ID_GPIO[0-3] = GPIO_NUMBER GPIO_IO_MUX GPIO_BANK_CTL
# GPIO_NUMBER: FCH GPIO number
# GPIO_IO_MUX: Value write to IOMUX to configure this GPIO
# GPIO_BANK_CTL: Value write to GPIOBankCtl[23:16] to configure this GPIO
# APCB_POPULATE_2ND_CHANNEL: Populates 2nd memory channel in APCB when true.
#	Trembyle based boards select 1 or 2 channels based on AGPIO84
#	Dalboz based boards only support 1 channel
APCB_BOARD_ID_GPIO0 = 121 1 0
APCB_BOARD_ID_GPIO1 = 120 1 0
APCB_BOARD_ID_GPIO2 = 131 3 0
APCB_BOARD_ID_GPIO3 = 116 1 0
APCB_POPULATE_2ND_CHANNEL = true

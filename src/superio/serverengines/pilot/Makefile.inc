# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-$(CONFIG_SUPERIO_SERVERENGINES_PILOT) += early_init.c
bootblock-$(CONFIG_SUPERIO_SERVERENGINES_PILOT) += early_serial.c
romstage-$(CONFIG_SUPERIO_SERVERENGINES_PILOT) += early_init.c
romstage-$(CONFIG_SUPERIO_SERVERENGINES_PILOT) += early_serial.c

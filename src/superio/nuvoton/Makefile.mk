# SPDX-License-Identifier: GPL-2.0-only

## include generic nuvoton pre-ram stage driver
bootblock-$(CONFIG_SUPERIO_NUVOTON_COMMON_PRE_RAM) += common/early_serial.c
romstage-$(CONFIG_SUPERIO_NUVOTON_COMMON_PRE_RAM) += common/early_serial.c

subdirs-$(CONFIG_SUPERIO_NUVOTON_WPCM450) += wpcm450
subdirs-$(CONFIG_SUPERIO_NUVOTON_NCT5104D) += nct5104d
subdirs-$(CONFIG_SUPERIO_NUVOTON_NCT5539D) += nct5539d
subdirs-$(CONFIG_SUPERIO_NUVOTON_NCT5572D) += nct5572d
subdirs-$(CONFIG_SUPERIO_NUVOTON_NCT6687D) += nct6687d
subdirs-$(CONFIG_SUPERIO_NUVOTON_NCT6776) += nct6776
subdirs-$(CONFIG_SUPERIO_NUVOTON_NCT6779D) += nct6779d
subdirs-$(CONFIG_SUPERIO_NUVOTON_NCT6791D) += nct6791d
subdirs-$(CONFIG_SUPERIO_NUVOTON_NPCD378) += npcd378

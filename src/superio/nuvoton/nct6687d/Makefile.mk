# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-$(CONFIG_SUPERIO_NUVOTON_NCT6687D) += nct6687d_ec.c
romstage-$(CONFIG_SUPERIO_NUVOTON_NCT6687D) += nct6687d_ec.c
ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT6687D) += nct6687d_ec.c

ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT6687D) += nct6687d_hwm.c
ramstage-$(CONFIG_SUPERIO_NUVOTON_NCT6687D) += superio.c

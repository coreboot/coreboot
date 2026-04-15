# SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_SUPERIO_MICROCHIP_MEC152x) += superio.c
romstage-$(CONFIG_SUPERIO_MICROCHIP_MEC152x) += early.c
bootblock-$(CONFIG_SUPERIO_MICROCHIP_MEC152x) += early.c

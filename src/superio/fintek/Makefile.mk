# SPDX-License-Identifier: GPL-2.0-only

## include generic fintek pre-ram stage driver
bootblock-$(CONFIG_SUPERIO_FINTEK_COMMON_PRE_RAM) += common/early_serial.c
romstage-$(CONFIG_SUPERIO_FINTEK_COMMON_PRE_RAM) += common/early_serial.c

subdirs-y += f71808a
subdirs-y += f71859
subdirs-y += f71863fg
subdirs-y += f71869ad
subdirs-y += f81865f
subdirs-y += f81866d
subdirs-y += f81803a
subdirs-y += f81966d

# SPDX-License-Identifier: GPL-2.0-only

## include generic nsc pre-ram stage driver
bootblock-$(CONFIG_SUPERIO_NSC_COMMON_PRE_RAM) += common/early_serial.c
romstage-$(CONFIG_SUPERIO_NSC_COMMON_PRE_RAM) += common/early_serial.c

subdirs-y += pc87309
subdirs-y += pc87360
subdirs-y += pc87366
subdirs-y += pc87382
subdirs-y += pc87384
subdirs-y += pc87392
subdirs-y += pc87417
subdirs-y += pc97317

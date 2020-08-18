# SPDX-License-Identifier: GPL-2.0-only

## include generic winbond pre-ram stage driver
bootblock-$(CONFIG_SUPERIO_WINBOND_COMMON_PRE_RAM) += common/early_init.c
romstage-$(CONFIG_SUPERIO_WINBOND_COMMON_PRE_RAM) += common/early_init.c

subdirs-y += w83627dhg
subdirs-y += w83627ehg
subdirs-y += w83627hf
subdirs-y += w83627thg
subdirs-y += w83627uhg
subdirs-y += w83667hg-a
subdirs-y += w83977tf
subdirs-y += wpcd376i

## SPDX-License-Identifier: GPL-2.0-only

libgcc_files = ashldi3.S lib1funcs.S lshrdi3.S muldi3.S ucmpdi2.S uldivmod.S ldivmod.S
libgcc_files += udivmoddi4.c umoddi3.c

ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARM),y)
decompressor-y += $(libgcc_files)
bootblock-y += $(libgcc_files)
endif

ifeq ($(CONFIG_ARCH_VERSTAGE_ARM),y)
verstage-y += $(libgcc_files)
endif

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM),y)
romstage-y += $(libgcc_files)
endif

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARM),y)
ramstage-y += $(libgcc_files)
endif

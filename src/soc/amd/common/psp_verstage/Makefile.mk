# SPDX-License-Identifier: GPL-2.0-only

verstage-generic-ccopts += -I$(src)/soc/amd/common/psp_verstage/include
verstage-generic-ccopts += -D__USER_SPACE__
CPPFLAGS_common += -I$(VBOOT_SOURCE)/firmware/2lib/include/

ifeq ($(CONFIG_COMPILER_GCC),y)
# This size should match the size in the linker script.
CFLAGS_arm += -Wstack-usage=40960
else
CFLAGS_arm += -Wframe-larger-than=40960
# Cezanne only has 148KB SRAM for PSP verstage.
# Add -Oz to reduce the verstage size for clang.
CFLAGS_arm += -Oz
endif

verstage-y += boot_dev.c
verstage-y += delay.c
verstage-y += fch.c
verstage-y += pmutil.c
verstage-y += post.c
verstage-y += printk.c
verstage-y += psp_verstage.c
verstage-y += psp.c
verstage-y += reset.c
verstage-y += timer.c
verstage-y += vboot_crypto.c

$(obj)/psp_verstage.bin: $(objcbfs)/verstage.elf
	$(OBJCOPY_verstage) -O binary $^ $@

# Default CONFIG_PSP_VERSTAGE_FILE configuration requires psp_verstage.bin and the above
# build rule takes effect. Once CONFIG_PSP_VERSTAGE_FILE is overridden with signed PSP
# verstage, then psp_verstage.bin is not built. The following build rule ensures that the
# unsigned psp_verstage.bin is still built even when not used so that it can be used for
# signing purposes.
build_complete:: $(obj)/psp_verstage.bin

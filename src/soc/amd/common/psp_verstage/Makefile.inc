# SPDX-License-Identifier: GPL-2.0-only

verstage-generic-ccopts += -I$(src)/soc/amd/common/psp_verstage/include
verstage-generic-ccopts += -D__USER_SPACE__
CPPFLAGS_common += -I$(VBOOT_SOURCE)/firmware/2lib/include/

ifeq ($(CONFIG_COMPILER_GCC),y)
# This size should match the size in the linker script.
CFLAGS_arm += -Wstack-usage=40960
else
CFLAGS_arm += -Wframe-larger-than=40960
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

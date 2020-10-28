# SPDX-License-Identifier: GPL-2.0-only

verstage-generic-ccopts += -I$(src)/soc/amd/picasso/psp_verstage/include
verstage-generic-ccopts += -I$(src)/vendorcode/amd/fsp/picasso/include
verstage-generic-ccopts += -D__USER_SPACE__
CPPFLAGS_common += -I$(VBOOT_SOURCE)/firmware/2lib/include/

# This size should match the size in the linker script.
CFLAGS_arm += -Wstack-usage=40960

verstage-y += delay.c
verstage-y += fch.c
verstage-y += pmutil.c
verstage-y += post.c
verstage-y += printk.c
verstage-y += psp_verstage.c
verstage-y += psp.c
verstage-y += reset.c
verstage-y += svc.c
verstage-y += timer.c
verstage-y += vboot_crypto.c

verstage-y += $(top)/src/vendorcode/amd/fsp/picasso/bl_uapp/bl_uapp_startup.S
verstage-y += $(top)/src/vendorcode/amd/fsp/picasso/bl_uapp/bl_uapp_end.S

$(obj)/psp_verstage.bin: $(objcbfs)/verstage.elf
	$(OBJCOPY_verstage) -O binary $^ $@

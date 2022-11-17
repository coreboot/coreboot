## SPDX-License-Identifier: GPL-2.0-only

armv7_flags = -mthumb -I$(src)/arch/arm/include/armv7/ -D__COREBOOT_ARM_ARCH__=7
armv7-a_flags = -march=armv7-a $(armv7_flags) -D__COREBOOT_ARM_V7_A__
armv7-m_flags = -march=armv7-m $(armv7_flags) -D__COREBOOT_ARM_V7_M__
armv7-r_flags = -march=armv7-r $(armv7_flags) -D__COREBOOT_ARM_V7_R__

armv7_asm_flags = -Wa,-mthumb -Wa,-mimplicit-it=always
ifeq ($(CONFIG_COMPILER_GCC),y)
armv7_asm_flags += -Wa,-mno-warn-deprecated
else # CLANG
bootblock-ld-ccopts += -target arm-eabi
verstage-ld-ccopts += -target arm-eabi
romstage-ld-ccopts += -target arm-eabi
ramstage-ld-ccopts += -target arm-eabi
rmodule_arm-ld-ccopts += -target arm-eabi
endif
armv7-r_asm_flags = $(armv7-r_flags) $(armv7_asm_flags)

###############################################################################
# bootblock
###############################################################################

ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARMV7),y)
decompressor-generic-ccopts += $(armv7-a_flags)
decompressor-S-ccopts += $(armv7_asm_flags)
bootblock-generic-ccopts += $(armv7-a_flags)
bootblock-S-ccopts += $(armv7_asm_flags)

ifneq ($(CONFIG_BOOTBLOCK_CUSTOM),y)
decompressor-y += bootblock.S
ifneq ($(CONFIG_COMPRESS_BOOTBLOCK),y)
bootblock-y += bootblock.S
endif
endif

decompressor-y += cache.c
bootblock-y += cache.c
decompressor-y += cpu.S
bootblock-y += cpu.S
decompressor-y += mmu.c
bootblock-y += mmu.c

bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += exception.c
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += exception_asm.S

else ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARMV7_M),y)
bootblock-generic-ccopts += $(armv7-m_flags)
bootblock-S-ccopts += $(armv7_asm_flags)

ifneq ($(CONFIG_BOOTBLOCK_CUSTOM),y)
bootblock-y += bootblock_m.S
endif
bootblock-y += exception_mr.c
bootblock-y += cache_m.c

else ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARMV7_R),y)
bootblock-generic-ccopts += $(armv7-r_flags)
bootblock-S-ccopts += $(armv7-r_asm_flags)

ifneq ($(CONFIG_BOOTBLOCK_CUSTOM),y)
bootblock-y += bootblock.S
endif

bootblock-y += cache.c
bootblock-y += cpu.S
bootblock-y += exception_mr.c
bootblock-y += mmu.c

endif # CONFIG_ARCH_BOOTBLOCK_ARMV7

################################################################################
## verification stage
################################################################################

ifeq ($(CONFIG_ARCH_VERSTAGE_ARMV7),y)
verstage-generic-ccopts += $(armv7-a_flags)
verstage-S-ccopts += $(armv7_asm_flags)

verstage-y += cache.c
verstage-y += cpu.S
verstage-y += exception.c
verstage-y += exception_asm.S
verstage-y += mmu.c

else ifeq ($(CONFIG_ARCH_VERSTAGE_ARMV7_M),y)
verstage-generic-ccopts += $(armv7-m_flags)
verstage-S-ccopts += $(armv7_asm_flags)

else ifeq ($(CONFIG_ARCH_VERSTAGE_ARMV7_R),y)
verstage-generic-ccopts += $(armv7-r_flags)
verstage-S-ccopts += $(armv7-r_asm_flags)

verstage-y += cache.c
verstage-y += cpu.S
verstage-y += exception_mr.c
verstage-y += mmu.c

endif # CONFIG_ARCH_VERSTAGE_ARMV7_M

################################################################################
## ROM stage
################################################################################
ifeq ($(CONFIG_ARCH_ROMSTAGE_ARMV7),y)
romstage-y += cache.c
romstage-y += cpu.S
romstage-y += exception.c
romstage-y += exception_asm.S
romstage-y += mmu.c

romstage-generic-ccopts += $(armv7-a_flags)
romstage-S-ccopts += $(armv7_asm_flags)

rmodules_arm-generic-ccopts += $(armv7-a_flags)
rmodules_arm-S-ccopts += $(armv7_asm_flags)

else ifeq ($(CONFIG_ARCH_ROMSTAGE_ARMV7_R),y)
romstage-y += cache.c
romstage-y += cpu.S
romstage-y += exception_mr.c
romstage-y += mmu.c

romstage-generic-ccopts += $(armv7-r_flags)
romstage-S-ccopts += $(armv7-r_asm_flags)

rmodules_arm-generic-ccopts += $(armv7-r_flags)
rmodules_arm-S-ccopts += $(armv7-r_asm_flags)

endif # CONFIG_ARCH_ROMSTAGE_ARMV7

###############################################################################
# ramstage
###############################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARMV7),y)

ramstage-y += cache.c
ramstage-y += cpu.S
ramstage-y += exception.c
ramstage-y += exception_asm.S
ramstage-y += mmu.c

ramstage-generic-ccopts += $(armv7-a_flags)
ramstage-S-ccopts += $(armv7_asm_flags)

# All rmodule code is armv7 if ramstage is armv7.
rmodules_arm-generic-ccopts += $(armv7-a_flags)
rmodules_arm-S-ccopts += $(armv7_asm_flags)

else ifeq ($(CONFIG_ARCH_RAMSTAGE_ARMV7_R),y)

ramstage-y += cache.c
ramstage-y += cpu.S
ramstage-y += exception_mr.c
ramstage-y += mmu.c

ramstage-generic-ccopts += $(armv7-r_flags)
ramstage-S-ccopts += $(armv7-r_asm_flags)

# All rmodule code is armv7 if ramstage is armv7.
rmodules_arm-generic-ccopts += $(armv7-r_flags)
rmodules_arm-S-ccopts += $(armv7-r_asm_flags)

endif # CONFIG_ARCH_RAMSTAGE_ARMV7

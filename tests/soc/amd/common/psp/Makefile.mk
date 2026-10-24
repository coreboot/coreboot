# SPDX-License-Identifier: GPL-2.0-only

tests-y += mailbox-legacy-test mailbox-ab-recovery-test
tests-y += mailbox-backup-flash-test

mailbox-legacy-test-srcs += tests/soc/amd/common/psp/mailbox-test.c
mailbox-legacy-test-srcs += src/soc/amd/common/block/psp/psp_gen2.c
mailbox-legacy-test-srcs += tests/stubs/console.c
mailbox-legacy-test-cflags += -Isrc/soc/amd/common/block/include
mailbox-legacy-test-cflags += -Isrc/soc/amd/common/block/psp
mailbox-legacy-test-cflags += -Itests/include/tests/lib
mailbox-legacy-test-config += CONFIG_ROM_SIZE=0x1000000
mailbox-legacy-test-config += CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2=1
mailbox-legacy-test-config += CONFIG_PSPV2_MBOX_CMD_OFFSET=0x10970

$(call copy-test,mailbox-legacy-test,mailbox-ab-recovery-test)
mailbox-ab-recovery-test-config += CONFIG_PSP_AB_RECOVERY=1

$(call copy-test,mailbox-legacy-test,mailbox-backup-flash-test)
mailbox-backup-flash-test-config += CONFIG_SOC_AMD_COMMON_BLOCK_SPI_BACKUP_SPI_FLASH=1

tests-y += boot-done-legacy-test

boot-done-legacy-test-srcs += tests/soc/amd/common/psp/boot-done-test.c
boot-done-legacy-test-srcs += tests/soc/amd/common/psp/boot-done-main.c
boot-done-legacy-test-srcs += src/soc/amd/common/block/psp/psp.c
boot-done-legacy-test-srcs += tests/stubs/console.c
boot-done-legacy-test-cflags += -Isrc/soc/amd/common/block/include
boot-done-legacy-test-cflags += -Isrc/soc/amd/common/block/psp
boot-done-legacy-test-cflags += -Wl,-T,tests/soc/amd/common/psp/bootstate.ld
boot-done-legacy-test-config += CONFIG_ROM_SIZE=0x2000000
boot-done-legacy-test-config += CONFIG_SOC_AMD_COMMON_BLOCK_PSP_GEN2=1

tests-y += boot-done-smm-lock-test
$(call copy-test,boot-done-legacy-test,boot-done-smm-lock-test)
boot-done-smm-lock-test-config += CONFIG_SOC_AMD_COMMON_BLOCK_PSP_SMM_LOCK=1
boot-done-smm-lock-test-config += CONFIG_HAVE_SMI_HANDLER=1

tests-y += boot-done-df-lock-test boot-done-locks-test
$(call copy-test,boot-done-legacy-test,boot-done-df-lock-test)
boot-done-df-lock-test-config += CONFIG_SOC_AMD_COMMON_BLOCK_PSP_DF_LOCK=1

$(call copy-test,boot-done-smm-lock-test,boot-done-locks-test)
boot-done-locks-test-config += CONFIG_SOC_AMD_COMMON_BLOCK_PSP_DF_LOCK=1

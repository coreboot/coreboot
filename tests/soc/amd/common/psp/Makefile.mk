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


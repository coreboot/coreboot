/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_PSP_H
#define AMD_BLOCK_PSP_H

#include <stdint.h>

#define SMM_TRIGGER_IO		0
#define SMM_TRIGGER_MEM		1

#define SMM_TRIGGER_BYTE	0
#define SMM_TRIGGER_WORD	1
#define SMM_TRIGGER_DWORD	2

struct smm_trigger_info {
	uint64_t address;	/* Memory or IO address */
	uint32_t address_type;	/* 0=I/O, 1=memory */
	uint32_t value_width;	/* 0=byte, 1=word, 2=qword */
	uint32_t value_and_mask;
	uint32_t value_or_mask;
} __packed;

struct smm_register {
	uint64_t address;	/* Memory or IO address */
	uint32_t address_type;	/* 0=I/O, 1=memory */
	uint32_t value_width;	/* 0=byte, 1=word, 2=qword */
	uint32_t reg_bit_mask;
	uint32_t expect_value;
} __packed;

struct smm_register_info {
	struct smm_register smi_enb;
	struct smm_register eos;
	struct smm_register psp_smi_en;
	struct smm_register reserved[5];
} __packed;

void soc_fill_smm_trig_info(struct smm_trigger_info *trig);
void soc_fill_smm_reg_info(struct smm_register_info *reg); /* v2 only */

/* BIOS-to-PSP functions return 0 if successful, else negative value */
#define PSPSTS_SUCCESS		0
#define PSPSTS_NOBASE		1
#define PSPSTS_HALTED		2
#define PSPSTS_RECOVERY		3
#define PSPSTS_SEND_ERROR	4
#define PSPSTS_INIT_TIMEOUT	5
#define PSPSTS_CMD_TIMEOUT	6
/* other error codes */
#define PSPSTS_UNSUPPORTED	7
#define PSPSTS_INVALID_NAME	8
#define PSPSTS_INVALID_BLOB	9

/* PSP gen1-only. SoCs with PSP gen2 already have the DRAM initialized when
   the x86 cores are released from reset. */
int psp_notify_dram(void);

int psp_notify_smm(void);

/*
 * type: identical to the corresponding PSP command, e.g. pass
 *       MBOX_BIOS_CMD_SMU_FW2 to load SMU FW2 blob.
 * name: cbfs file name
 */
enum psp_blob_type {
	BLOB_SMU_FW,
	BLOB_SMU_FW2,
};

/*
 * Notify PSP that the system is entering a sleep state.  sleep_state uses the
 * same definition as Pm1Cnt[SlpTyp], typically 0, 1, 3, 4, 5.
 */
void psp_notify_sx_info(u8 sleep_type);

int psp_load_named_blob(enum psp_blob_type type, const char *name);

/* Sets the GPIO used for the TPM IRQ */
void psp_set_tpm_irq_gpio(unsigned int gpio);

#endif /* AMD_BLOCK_PSP_H */

/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __AMD_PSP_DEF_H__
#define __AMD_PSP_DEF_H__

#include <types.h>
#include <commonlib/helpers.h>
#include <amdblocks/psp.h>

#define CORE_2_PSP_MSG_38_OFFSET	0x10998 /* 4 byte */
#define   CORE_2_PSP_MSG_38_FUSE_SPL		BIT(12)
#define   CORE_2_PSP_MSG_38_SPL_FUSE_ERROR	BIT(13)
#define   CORE_2_PSP_MSG_38_SPL_ENTRY_ERROR	BIT(14)
#define   CORE_2_PSP_MSG_38_SPL_ENTRY_MISSING	BIT(15)

/* x86 to PSP commands */
#define MBOX_BIOS_CMD_SMM_INFO			0x02
#define MBOX_BIOS_CMD_SX_INFO			0x03
#define   MBOX_BIOS_CMD_SX_INFO_SLEEP_TYPE_MAX	0x07
#define MBOX_BIOS_CMD_RSM_INFO			0x04
#define MBOX_BIOS_CMD_PSP_FTPM_QUERY		0x05
#define   MBOX_FTPM_CAP_TPM_SUPPORTED		(1 << 0)
#define   MBOX_FTPM_CAP_TPM_REQ_FACTORY_RESET	(1 << 1)
#define   MBOX_FTPM_CAP_FTPM_NEED_RECOVER	(1 << 2)
#define MBOX_BIOS_CMD_BOOT_DONE			0x06
#define MBOX_BIOS_CMD_CLEAR_S3_STS		0x07
#define MBOX_BIOS_CMD_S3_DATA_INFO		0x08
#define MBOX_BIOS_CMD_NOP			0x09
#define MBOX_BIOS_CMD_HSTI_QUERY		0x14
#define  HSTI_STATE_ROM_ARMOR_ENFORCED		BIT(11)
#define MBOX_BIOS_CMD_PSB_AUTO_FUSING		0x21
#define MBOX_BIOS_CMD_SET_BOOTPARTITION		0x26
#define MBOX_BIOS_CMD_GET_BOOTPARTITION		0x36
#define MBOX_BIOS_CMD_PSP_CAPS_QUERY		0x27
#define MBOX_BIOS_CMD_SET_SPL_FUSE		0x2d
#define MBOX_BIOS_CMD_SET_RPMC_ADDRESS		0x39
#define MBOX_BIOS_CMD_QUERY_SPL_FUSE		0x47
#define MBOX_BIOS_CMD_ARMOR_ENTER_SMM_MODE	0x50
#define MBOX_BIOS_CMD_ARMOR_SPI_TRANSACTION	0x51
#define MBOX_BIOS_CMD_I2C_TPM_ARBITRATION	0x64
#define MBOX_BIOS_CMD_ABORT			0xfe

/* x86 to PSP commands, v1-only */
#define MBOX_BIOS_CMD_DRAM_INFO			0x01
#define MBOX_BIOS_CMD_SMU_FW			0x19
#define MBOX_BIOS_CMD_SMU_FW2			0x1a

#define SMN_PSP_PUBLIC_BASE			0x3800000

/* command/response format, BIOS builds this in memory
 *   mbox_buffer_header: generic header
 *   mbox_buffer:        command-specific buffer format
 *
 * AMD reference code aligns and pads all buffers to 32 bytes.
 */
struct mbox_buffer_header {
	uint32_t size;		/* total size of buffer */
	uint32_t status;	/* command status, filled by PSP if applicable */
} __packed;

/*
 * x86 to PSP mailbox commands that don't take any parameter or return any data, use the
 * mbox_default_buffer, while x86 to PSP commands that either pass data to the PSP or get data
 * returned from the PSP use command-specific buffer definitions. For details on the specific
 * buffer definitions for the various commands, see NDA document #54267 for the generations
 * before family 17h and NDA document #55758 for the generations from family 17h on.
 */

struct mbox_default_buffer {	/* command-response buffer unused by command */
	struct mbox_buffer_header header;
} __packed __aligned(32);

struct smm_req_buffer {
	uint64_t smm_base;		/* TSEG base */
	uint64_t smm_mask;		/* TSEG mask */
	uint64_t psp_smm_data_region;	/* PSP region in SMM space */
	uint64_t psp_smm_data_length;	/* PSP region length in SMM space */
	struct smm_trigger_info smm_trig_info;
#if CONFIG(SOC_AMD_COMMON_BLOCK_PSP_GEN2)
	struct smm_register_info smm_reg_info;
#endif
	uint64_t psp_mbox_smm_buffer_address;
	uint64_t psp_mbox_smm_flag_address;
} __packed;

/* MBOX_BIOS_CMD_SMM_INFO */
struct mbox_cmd_smm_info_buffer {
	struct mbox_buffer_header header;
	struct smm_req_buffer req;
} __packed __aligned(32);

/* MBOX_BIOS_CMD_SX_INFO */
struct mbox_cmd_sx_info_buffer {
	struct mbox_buffer_header header;
	uint8_t sleep_type;
} __packed __aligned(32);

/* MBOX_BIOS_CMD_PSP_FTPM_QUERY, MBOX_BIOS_CMD_PSP_CAPS_QUERY */
struct mbox_cmd_capability_query_buffer {
	struct mbox_buffer_header header;
	uint32_t capabilities;
} __packed __aligned(32);

/* MBOX_BIOS_CMD_HSTI_QUERY */
struct mbox_cmd_hsti_query_buffer {
	struct mbox_buffer_header header;
	uint32_t state;
} __packed __aligned(32);

/* MBOX_BIOS_CMD_SET_RPMC_ADDRESS */
struct mbox_cmd_set_rpmc_address_buffer {
	struct mbox_buffer_header header;
	uint32_t address;
} __packed __aligned(32);

/* MBOX_BIOS_CMD_SET_SPL_FUSE */
struct mbox_cmd_late_spl_buffer {
	struct mbox_buffer_header header;
	uint32_t	spl_value;
} __packed __aligned(32);

/*
 * MBOX_BIOS_CMD_SET_BOOTPARTITION,
 * MBOX_BIOS_CMD_GET_BOOTPARTITION
 *
 * boot_partition is typically 0 or 1.
 */
struct mbox_cmd_boot_partition_buffer {
	struct mbox_buffer_header header;
	uint32_t	boot_partition;
} __packed __aligned(32);

struct dtpm_config {
	uint32_t gpio;
} __packed;

enum dtpm_request_type {
	DTPM_REQUEST_ACQUIRE,	/* Acquire I2C bus */
	DTPM_REQUEST_RELEASE,	/* Release I2C bus */
	DTPM_REQUEST_CONFIG,	/* Provide DTPM info */
	DTPM_REQUEST_MAX,
};

/* MBOX_BIOS_CMD_I2C_TPM_ARBITRATION */
struct mbox_cmd_dtpm_config_buffer {
	struct mbox_buffer_header header;
	uint32_t request_type;
	struct dtpm_config config;
} __packed __aligned(32);

/* MBOX_BIOS_CMD_ARMOR_ENTER_SMM_MODE */
struct mbox_rom_armor_enforce_buffer {
	struct mbox_buffer_header header;
	uint32_t flash_size;		/* Returned by PSP: SPI flash size in bytes */
	uint32_t capsule_update;	/* 1 for capsule update/recovery mode, 0 otherwise */
} __packed __aligned(32);

enum mbox_rom_armor_transaction {
	RA_READ		= 1,
	RA_WRITE	= 2,
	RA_ERASE	= 3,
};

struct mbox_rom_armor_flash_command {
	enum mbox_rom_armor_transaction transaction;
	uint64_t buffer_ptr;	/* Pointer to data buffer. Must not be NULL. */
	uint32_t offset;	/* SPI flash offset */
	uint32_t size;		/* Transfer size for all operations */
	uint32_t read_back;	/* Whether to read back data after write for validation */
} __packed;

struct mbox_rom_armor_flash_command_buffer {
	struct mbox_buffer_header header;
	struct mbox_rom_armor_flash_command cmd;
} __packed __aligned(32);

#define PSP_INIT_TIMEOUT 10000 /* 10 seconds */
#define PSP_CMD_TIMEOUT 1000 /* 1 second */

#define C2P_BUFFER_MAXSIZE 0xc00 /* Core-to-PSP buffer */
#define P2C_BUFFER_MAXSIZE 0x1000 /* PSP-to-core buffer */

/* PSP to x86 status */
enum mbox_p2c_status {
	MBOX_PSP_SUCCESS		= 0x00,
	MBOX_PSP_INVALID_PARAMETER	= 0x01,
	MBOX_PSP_CRC_ERROR		= 0x02,
	/*
	 * Send to PSP when the requested SPI command in the psp_smi_handler()
	 * handler failed due to an unknown error. The PSP usually doesn't like
	 * seeing this return code and will stop operating.
	 */
	 MBOX_PSP_COMMAND_PROCESS_ERROR	= 0x04,
	 MBOX_PSP_UNSUPPORTED		= 0x08,
	 MBOX_PSP_SPI_BUSY_ASYNC	= 0x0a,
	 /*
	  * Send to PSP when the requested SPI command in the psp_smi_handler()
	  * handler cannot be executed right away. This can happen when the SPI
	  * flash is busy or the SPI controller is busy or being used by ring 0.
	  *
	  * The PSP will raise an SMI later again.
	  */
	 MBOX_PSP_SPI_BUSY		= 0x0b,
};

/*
 * PSP register base retrieval and register access functions used by the PSP v2
 * code. Those are used to do the register access depending on the stage either
 * via SMN or via MMIO, sinc ethe MMIO access method is only available later in
 * boot while the SMN access will already work before that, but has a bit more
 * overhead compared to MMIO.
 */
uint64_t psp_get_base(void);
uint32_t psp_read32(uint64_t base, uint32_t offset);
void psp_write32(uint64_t base, uint32_t offset, uint32_t data);
void psp_write64(uint64_t base, uint32_t offset, uint64_t data);

void psp_print_cmd_status(int cmd_status, struct mbox_buffer_header *header);

/* This command needs to be implemented by the generation specific code. */
int send_psp_command(uint32_t command, void *buffer);

enum cb_err psp_get_ftpm_capabilties(uint32_t *capabilities);
enum cb_err psp_get_psp_capabilities(uint32_t *capabilities);
enum cb_err psp_get_hsti_state(uint32_t *state);
enum cb_err soc_read_c2p38(uint32_t *msg_38_value);

void enable_psp_smi(void);

void psp_set_smm_flag(void);
void psp_clear_smm_flag(void);

/* psp_ab_recovery_set_bootpartition - Set active partition on next boot.
 * @param partition: Active partition on next boot. 0: A, 1: B.
 * @return 0 on success
 */
int psp_ab_recovery_set_bootpartition(const uint32_t partition);

/* psp_ab_recovery_get_bootpartition - Get active partition on next boot.
 * @return negative on failure. 0 if A is active boot partition, 1 if B is active
 *         boot partition.
 */
int psp_ab_recovery_get_bootpartition(void);

struct mbox_rom_armor_flash_command;
/*
 * psp_rom_armor_spi_transaction - Send PSP ROM Armor SPI transaction command to PSP firmware
 *
 * @param cmd_buf: Command buffer with SPI transaction to execute.
 *
 * On ROM Armor2:
 * READ/WRITE/ERASE
 *
 * On ROM Armor3:
 * WRITE/ERASE
 * (READ is not supported on ROM Armor3 as flash contents can be read directly from MMIO)
 *
 * Communicates with PSP via mailbox to perform the requested SPI flash operation through ROM Armor.
 * The PSP firmware will enforce the access based on the command parameters and the
 * protection configuration in PSP firmware ('Writable' bit set on PSP directory entries) and
 * BIOS directory types 0x6d (whitelisted flash regions) and return the result via the same command buffer.
 *
 * Returns 0 on success, negative error code on failure.
 */
int psp_rom_armor_spi_transaction(const struct mbox_rom_armor_flash_command *cmd_buf);

/**
 * psp_rom_armor_enter_smm_mode - Active PSP Rom Armor
 * @param allow_capsule_update: Indicates if the system is in capsule update mode
 * @param flash_size: Pointer to store the flash size retrieved from PSP firmware
 *
 * After this function is called, PSP Rom Armor will be active and protect
 * the SPI flash according to the configuration in PSP firmware. This function
 * also retrieves the flash size from PSP firmware and returns it via the
 * flash_size output parameter.
 *
 * Returns: 0 on success, negative error code on failure
 */
int psp_rom_armor_enter_smm_mode(const bool allow_capsule_update, size_t *flash_size);

#endif /* __AMD_PSP_DEF_H__ */

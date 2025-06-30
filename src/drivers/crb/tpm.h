/* SPDX-License-Identifier: BSD-3-Clause */
/* This is a driver for a Command Response Buffer Interface */

#include <security/tpm/tis.h>
#include <security/tpm/tss_errors.h>

/* CRB driver */
/* address of locality 0 (CRB) */
#define TPM_CRB_BASE_ADDRESS        CONFIG_CRB_TPM_BASE_ADDRESS

#define CRB_REG(LOCTY, REG)                                 \
	(void *)(uintptr_t)(CONFIG_CRB_TPM_BASE_ADDRESS + (LOCTY << 12) + REG)

/* hardware registers */
#define CRB_REG_LOC_STATE		0x00
#define CRB_REG_LOC_CTRL		0x08
#define CRB_REG_LOC_STS			0x0C

/* LOC_CTRL BIT MASKS */
#define LOC_CTRL_REQ_ACCESS		0x01

/* LOC STATE BIT MASKS */
#define LOC_STATE_LOC_ASSIGN	0x02
#define LOC_STATE_REG_VALID_STS	0x80

/* LOC STS BIT MASKS */
#define LOC_STS_GRANTED			0x01

#define CRB_REG_INTF_ID			0x30
#define CRB_REG_REQUEST			0x40
#define CRB_REG_STATUS			0x44
#define CRB_REG_CANCEL			0x48
#define CRB_REG_START			0x4C
#define CRB_REG_INT_CTRL		0x50
#define CRB_REG_CMD_SIZE		0x58
#define CRB_REG_CMD_ADDR		0x5C
#define CRB_REG_RESP_SIZE		0x64
#define CRB_REG_RESP_ADDR		0x68
#define CRB_REG_DATA_BUFF		0x80

/* CRB INTF BIT MASK */
#define CRB_INTF_REG_CAP_CRB	(1<<14)
#define CRB_INTF_REG_INTF_SEL	(1<<17)
#define CRB_INTF_REG_INTF_LOCK	(1<<19)

/*REQUEST Register related */
#define CRB_REG_REQUEST_CMD_RDY		0x01
#define CRB_REG_REQUEST_GO_IDLE		0x02

/* STATUS Register related */
#define CRB_REG_STATUS_ERROR		0x01
#define CRB_REG_STATUS_IDLE			0x02

/* START Register related */
#define CRB_REG_START_START			0x01

/* CRB TPM Info Struct */
struct crb_tpm_info {
	uint16_t vendor_id;
	uint16_t device_id;
	uint16_t revision;
};

tpm_result_t crb_tpm_init(void);
void crb_tpm_get_info(struct crb_tpm_info *crb_tpm_info);
size_t crb_tpm_process_command(const void *tpm2_command, size_t command_size,
			       void *tpm2_response, size_t max_response);
bool crb_tpm_is_active(void);

tis_sendrecv_fn crb_tis_probe(enum tpm_family *family);

uintptr_t crb_tpm_base_address(void);

/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef ME_H
#define ME_H

#include <inttypes.h>
#include <pci/pci.h>

#define ME_RETRY                100000  /* 1 second */
#define ME_DELAY                10      /* 10 us */

#pragma pack(1)

/*
 * Management Engine PCI registers
 */

#define PCI_ME_HFS		0x40
#define  ME_HFS_CWS_RESET	0
#define  ME_HFS_CWS_INIT	1
#define  ME_HFS_CWS_REC		2
#define  ME_HFS_CWS_NORMAL	5
#define  ME_HFS_CWS_WAIT	6
#define  ME_HFS_CWS_TRANS	7
#define  ME_HFS_CWS_INVALID	8
#define  ME_HFS_STATE_PREBOOT	0
#define  ME_HFS_STATE_M0_UMA	1
#define  ME_HFS_STATE_M3	4
#define  ME_HFS_STATE_M0	5
#define  ME_HFS_STATE_BRINGUP	6
#define  ME_HFS_STATE_ERROR	7
#define  ME_HFS_ERROR_NONE	0
#define  ME_HFS_ERROR_UNCAT	1
#define  ME_HFS_ERROR_IMAGE	3
#define  ME_HFS_ERROR_DEBUG	4
#define  ME_HFS_MODE_NORMAL	0
#define  ME_HFS_MODE_DEBUG	2
#define  ME_HFS_MODE_DIS	3
#define  ME_HFS_MODE_OVER_JMPR	4
#define  ME_HFS_MODE_OVER_MEI	5
#define  ME_HFS_BIOS_DRAM_ACK	1
#define  ME_HFS_ACK_NO_DID	0
#define  ME_HFS_ACK_RESET	1
#define  ME_HFS_ACK_PWR_CYCLE	2
#define  ME_HFS_ACK_S3		3
#define  ME_HFS_ACK_S4		4
#define  ME_HFS_ACK_S5		5
#define  ME_HFS_ACK_GBL_RESET	6
#define  ME_HFS_ACK_CONTINUE	7

struct me_hfs {
	uint32_t working_state: 4;
	uint32_t mfg_mode: 1;
	uint32_t fpt_bad: 1;
	uint32_t operation_state: 3;
	uint32_t fw_init_complete: 1;
	uint32_t ft_bup_ld_flr: 1;
	uint32_t update_in_progress: 1;
	uint32_t error_code: 4;
	uint32_t operation_mode: 4;
	uint32_t reserved: 4;
	uint32_t boot_options_present: 1;
	uint32_t ack_data: 3;
	uint32_t bios_msg_ack: 4;
} __attribute__((packed));

#define PCI_ME_UMA		0x44

struct me_uma {
	uint32_t size: 6;
	uint32_t reserved_1: 10;
	uint32_t valid: 1;
	uint32_t reserved_0: 14;
	uint32_t set_to_one: 1;
} __attribute__((packed));

#define PCI_ME_H_GS		0x4c
#define  ME_INIT_DONE		1
#define  ME_INIT_STATUS_SUCCESS	0
#define  ME_INIT_STATUS_NOMEM	1
#define  ME_INIT_STATUS_ERROR	2

struct me_did {
	uint32_t uma_base: 16;
	uint32_t reserved: 8;
	uint32_t status: 4;
	uint32_t init_done: 4;
} __attribute__((packed));

#define PCI_ME_GMES		0x48
#define  ME_GMES_PHASE_ROM	0
#define  ME_GMES_PHASE_BUP	1
#define  ME_GMES_PHASE_UKERNEL	2
#define  ME_GMES_PHASE_POLICY	3
#define  ME_GMES_PHASE_MODULE	4
#define  ME_GMES_PHASE_UNKNOWN	5
#define  ME_GMES_PHASE_HOST	6

struct me_gmes {
	uint32_t bist_in_prog : 1;
	uint32_t icc_prog_sts : 2;
	uint32_t invoke_mebx : 1;
	uint32_t cpu_replaced_sts : 1;
	uint32_t mbp_rdy : 1;
	uint32_t mfs_failure : 1;
	uint32_t warm_rst_req_for_df : 1;
	uint32_t cpu_replaced_valid : 1;
	uint32_t reserved_1 : 2;
	uint32_t fw_upd_ipu : 1;
	uint32_t reserved_2 : 4;
	uint32_t current_state: 8;
	uint32_t current_pmevent: 4;
	uint32_t progress_code: 4;
} __attribute__((packed));

#define PCI_ME_HERES		0xbc
#define  PCI_ME_EXT_SHA1	0x00
#define  PCI_ME_EXT_SHA256	0x02
#define PCI_ME_HER(x)		(0xc0+(4*(x)))

struct me_heres {
	uint32_t extend_reg_algorithm: 4;
	uint32_t reserved: 26;
	uint32_t extend_feature_present: 1;
	uint32_t extend_reg_valid: 1;
} __attribute__((packed));

struct me_thermal_reporting {
	uint32_t polling_timeout: 8;
	uint32_t smbus_ec_msglen: 8;
	uint32_t smbus_ec_msgpec: 8;
	uint32_t dimmnumber: 8;
} __attribute__((packed));

/*
 * Management Engine MEI registers
 */

#define MEI_H_CB_WW		0x00
#define MEI_H_CSR		0x04
#define MEI_ME_CB_RW		0x08
#define MEI_ME_CSR_HA		0x0c

struct mei_csr {
	uint32_t interrupt_enable: 1;
	uint32_t interrupt_status: 1;
	uint32_t interrupt_generate: 1;
	uint32_t ready: 1;
	uint32_t reset: 1;
	uint32_t reserved: 3;
	uint32_t buffer_read_ptr: 8;
	uint32_t buffer_write_ptr: 8;
	uint32_t buffer_depth: 8;
} __attribute__((packed));

#define MEI_ADDRESS_HBM		0x00
#define MEI_ADDRESS_CORE_WD	0x01
#define MEI_ADDRESS_AMT		0x02
#define MEI_ADDRESS_RESERVED	0x03
#define MEI_ADDRESS_WDT		0x04
#define MEI_ADDRESS_POLICY	0x05
#define MEI_ADDRESS_PASSWORD	0x06
#define MEI_ADDRESS_MKHI	0x07
#define MEI_ADDRESS_ICC		0x08
#define MEI_ADDRESS_THERMAL	0x09
#define MEI_ADDRESS_SPI		0x0a

#define MEI_HOST_ADDRESS	0

struct mei_header {
	uint32_t client_address: 8;
	uint32_t host_address: 8;
	uint32_t length: 9;
	uint32_t reserved: 6;
	uint32_t is_complete: 1;
} __attribute__((packed));

#define MKHI_GROUP_ID_CBM	0x00
#define MKHI_GROUP_ID_PM	0x01
#define MKHI_GROUP_ID_PWD	0x02
#define MKHI_GROUP_ID_FWCAPS	0x03
#define MKHI_GROUP_ID_APP	0x04
#define MKHI_GROUP_ID_SPI	0x05
#define MKHI_GROUP_ID_MDES	0x08
#define MKHI_GROUP_ID_MAX	0x09
#define MKHI_GROUP_ID_GEN	0xff

#define MKHI_FWCAPS_GET_RULE	0x02
#define MKHI_FWCAPS_SET_RULE	0x03
#define MKHI_GLOBAL_RESET	0x0b

#define GEN_GET_MKHI_VERSION	0x01
#define GEN_GET_FW_VERSION	0x02
#define GEN_UNCONFIG_NO_PWD	0x0d
#define GEN_SET_DEBUG_MEM	0x11

#define FWCAPS_ME_FWU_RULE	0x2e
#define FWCAPS_OVERRIDE		0x14

#define MKHI_THERMAL_REPORTING  0x00
#define MKHI_GET_FW_VERSION	0x02
#define MKHI_MDES_ENABLE	0x09
#define MKHI_END_OF_POST	0x0c
#define MKHI_FEATURE_OVERRIDE	0x14

#define HBM_HOST_START_REQ_CMD                  0x01
#define HBM_HOST_STOP_REQ_CMD                   0x02
#define HBM_ME_STOP_REQ_CMD                     0x03
#define HBM_HOST_ENUM_REQ_CMD                   0x04
#define HBM_HOST_CLIENT_PROPERTIES_REQ_CMD      0x05
#define HBM_CLIENT_CONNECT_REQ_CMD              0x06
#define HBM_CLIENT_DISCONNECT_REQ_CMD           0x07

struct mkhi_header {
	uint32_t group_id: 8;
	uint32_t command: 7;
	uint32_t is_response: 1;
	uint32_t reserved: 8;
	uint32_t result: 8;
} __attribute__((packed));

struct me_fw_version {
	uint16_t code_minor;
	uint16_t code_major;
	uint16_t code_build_number;
	uint16_t code_hot_fix;
	uint16_t recovery_minor;
	uint16_t recovery_major;
	uint16_t recovery_build_number;
	uint16_t recovery_hot_fix;
	uint16_t fitcminor;
	uint16_t fitcmajor;
	uint16_t fitcbuildno;
	uint16_t fitchotfix;
} __attribute__((packed));


#define HECI_EOP_STATUS_SUCCESS       0x0
#define HECI_EOP_PERFORM_GLOBAL_RESET 0x1

#define CBM_RR_GLOBAL_RESET	0x01

#define GLOBAL_RESET_BIOS_MRC	0x01
#define GLOBAL_RESET_BIOS_POST	0x02
#define GLOBAL_RESET_MEBX	0x03

struct me_global_reset {
	uint8_t request_origin;
	uint8_t reset_type;
} __attribute__((packed));

typedef enum {
	ME_NORMAL_BIOS_PATH,
	ME_S3WAKE_BIOS_PATH,
	ME_ERROR_BIOS_PATH,
	ME_RECOVERY_BIOS_PATH,
	ME_DISABLE_BIOS_PATH,
	ME_FIRMWARE_UPDATE_BIOS_PATH,
} me_bios_path;

typedef struct {
	uint32_t       major_version  : 16;
	uint32_t       minor_version  : 16;
	uint32_t       hotfix_version : 16;
	uint32_t       build_version  : 16;
} __attribute__((packed)) mbp_fw_version_name;

typedef struct {
	uint8_t        num_icc_profiles;
	uint8_t        icc_profile_soft_strap;
	uint8_t        icc_profile_index;
	uint8_t        reserved;
	uint32_t       register_lock_mask[3];
} __attribute__((packed)) mbp_icc_profile;

typedef struct {
	uint32_t  full_net		: 1;
	uint32_t  std_net		: 1;
	uint32_t  manageability	: 1;
	uint32_t  small_business	: 1;
	uint32_t  l3manageability	: 1;
	uint32_t  intel_at		: 1;
	uint32_t  intel_cls		: 1;
	uint32_t  reserved		: 3;
	uint32_t  intel_mpc		: 1;
	uint32_t  icc_over_clocking	: 1;
	uint32_t  pavp		: 1;
	uint32_t  reserved_1		: 4;
	uint32_t  ipv6		: 1;
	uint32_t  kvm		: 1;
	uint32_t  och		: 1;
	uint32_t  vlan		: 1;
	uint32_t  tls		: 1;
	uint32_t  reserved_4		: 1;
	uint32_t  wlan		: 1;
	uint32_t  reserved_5		: 8;
} __attribute__((packed)) mefwcaps_sku;

typedef struct {
	uint16_t  lock_state		     : 1;
	uint16_t  authenticate_module     : 1;
	uint16_t  s3authentication  	     : 1;
	uint16_t  flash_wear_out          : 1;
	uint16_t  flash_variable_security : 1;
	uint16_t  wwan3gpresent	     : 1;
	uint16_t  wwan3goob		     : 1;
	uint16_t  reserved		     : 9;
} __attribute__((packed)) tdt_state_flag;

typedef struct {
	uint8_t           state;
	uint8_t           last_theft_trigger;
	tdt_state_flag  flags;
}  __attribute__((packed)) tdt_state_info;

typedef struct {
	uint32_t  platform_target_usage_type	 : 4;
	uint32_t  platform_target_market_type : 2;
	uint32_t  super_sku			 : 1;
	uint32_t  reserved			 : 1;
	uint32_t  intel_me_fw_image_type	 : 4;
	uint32_t  platform_brand		 : 4;
	uint32_t  reserved_1			 : 16;
}  __attribute__((packed)) platform_type_rule_data;

typedef struct {
	mefwcaps_sku fw_capabilities;
	uint8_t      available;
} mbp_fw_caps;

typedef struct {
	uint16_t        device_id;
	uint16_t        fuse_test_flags;
	uint32_t        umchid[4];
}  __attribute__((packed)) mbp_rom_bist_data;

typedef struct {
	uint32_t        key[8];
} mbp_platform_key;

typedef struct {
	platform_type_rule_data rule_data;
	uint8_t	          available;
} mbp_plat_type;

typedef struct {
	mbp_fw_version_name fw_version_name;
	mbp_fw_caps	    fw_caps_sku;
	mbp_rom_bist_data   rom_bist_data;
	mbp_platform_key    platform_key;
	mbp_plat_type	    fw_plat_type;
	mbp_icc_profile	    icc_profile;
	tdt_state_info	    at_state;
	uint32_t		    mfsintegrity;
} me_bios_payload;

typedef  struct {
	uint32_t  mbp_size	 : 8;
	uint32_t  num_entries : 8;
	uint32_t  rsvd      	 : 16;
} __attribute__((packed)) mbp_header;

typedef struct {
	uint32_t  app_id  : 8;
	uint32_t  item_id : 8;
	uint32_t  length  : 8;
	uint32_t  rsvd    : 8;
}  __attribute__((packed)) mbp_item_header;

struct me_fwcaps {
	uint32_t id;
	uint8_t length;
	mefwcaps_sku caps_sku;
	uint8_t reserved[3];
} __attribute__((packed));

struct me_debug_mem {
	uint32_t debug_phys;
        uint32_t debug_size;
        uint32_t me_phys;
        uint32_t me_size;
} __attribute__((packed));

void intel_me_status(uint32_t hfs, uint32_t gmes);
void mkhi_thermal(void);
uint32_t intel_mei_setup(struct pci_dev *dev);
void intel_mei_unmap(void);
int mkhi_get_fwcaps(void);
int mkhi_get_fw_version(int *major, int *minor);
int mkhi_debug_me_memory(void *addr);
void mei_reset(void);
int intel_me_extend_valid(struct pci_dev *dev);

#endif

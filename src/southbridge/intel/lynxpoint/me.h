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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef _INTEL_ME_H
#define _INTEL_ME_H

#define ME_RETRY		100000	/* 1 second */
#define ME_DELAY		10	/* 10 us */

/*
 * Management Engine PCI registers
 */

#define PCI_CPU_DEVICE		PCI_DEV(0,0,0)
#define PCI_CPU_MEBASE_L	0x70	/* Set by MRC */
#define PCI_CPU_MEBASE_H	0x74	/* Set by MRC */

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
	u32 working_state: 4;
	u32 mfg_mode: 1;
	u32 fpt_bad: 1;
	u32 operation_state: 3;
	u32 fw_init_complete: 1;
	u32 ft_bup_ld_flr: 1;
	u32 update_in_progress: 1;
	u32 error_code: 4;
	u32 operation_mode: 4;
	u32 reserved: 4;
	u32 boot_options_present: 1;
	u32 ack_data: 3;
	u32 bios_msg_ack: 4;
} __attribute__ ((packed));

#define PCI_ME_UMA		0x44

struct me_uma {
	u32 size: 6;
	u32 reserved_1: 10;
	u32 valid: 1;
	u32 reserved_0: 14;
	u32 set_to_one: 1;
} __attribute__ ((packed));

#define PCI_ME_H_GS		0x4c
#define  ME_INIT_DONE		1
#define  ME_INIT_STATUS_SUCCESS	0
#define  ME_INIT_STATUS_NOMEM	1
#define  ME_INIT_STATUS_ERROR	2
#define  ME_INIT_STATUS_SUCCESS_OTHER 3 /* SEE ME9 BWG */

struct me_did {
	u32 uma_base: 16;
	u32 reserved: 7;
	u32 rapid_start: 1;
	u32 status: 4;
	u32 init_done: 4;
} __attribute__ ((packed));

/*
 * Apparently the GMES register is renamed to HFS2 (or HFSTS2 according
 * to ME9 BWG). Sadly the PCH EDS and the ME BWG do not match on nomenclature.
 */
#define PCI_ME_HFS2		0x48
/* Infrastructure Progress Values */
#define  ME_HFS2_PHASE_ROM		0
#define  ME_HFS2_PHASE_BUP		1
#define  ME_HFS2_PHASE_UKERNEL		2
#define  ME_HFS2_PHASE_POLICY		3
#define  ME_HFS2_PHASE_MODULE_LOAD	4
#define  ME_HFS2_PHASE_UNKNOWN		5
#define  ME_HFS2_PHASE_HOST_COMM	6
/* Current State - Based on Infra Progress values. */
/*       ROM State */
#define  ME_HFS2_STATE_ROM_BEGIN 0
#define  ME_HFS2_STATE_ROM_DISABLE 6
/*       BUP State */
#define  ME_HFS2_STATE_BUP_INIT 0
#define  ME_HFS2_STATE_BUP_DIS_HOST_WAKE 1
#define  ME_HFS2_STATE_BUP_FLOW_DET 4
#define  ME_HFS2_STATE_BUP_VSCC_ERR 8
#define  ME_HFS2_STATE_BUP_CHECK_STRAP 0xa
#define  ME_HFS2_STATE_BUP_PWR_OK_TIMEOUT 0xb
#define  ME_HFS2_STATE_BUP_MANUF_OVRD_STRAP 0xd
#define  ME_HFS2_STATE_BUP_M3 0x11
#define  ME_HFS2_STATE_BUP_M0 0x12
#define  ME_HFS2_STATE_BUP_FLOW_DET_ERR 0x13
#define  ME_HFS2_STATE_BUP_M3_CLK_ERR 0x15
#define  ME_HFS2_STATE_BUP_M3_KERN_LOAD 0x18
#define  ME_HFS2_STATE_BUP_T32_MISSING 0x1c
#define  ME_HFS2_STATE_BUP_WAIT_DID 0x1f
#define  ME_HFS2_STATE_BUP_WAIT_DID_FAIL 0x20
#define  ME_HFS2_STATE_BUP_DID_NO_FAIL 0x21
#define  ME_HFS2_STATE_BUP_ENABLE_UMA 0x22
#define  ME_HFS2_STATE_BUP_ENABLE_UMA_ERR 0x23
#define  ME_HFS2_STATE_BUP_SEND_DID_ACK 0x24
#define  ME_HFS2_STATE_BUP_SEND_DID_ACK_ERR 0x25
#define  ME_HFS2_STATE_BUP_M0_CLK 0x26
#define  ME_HFS2_STATE_BUP_M0_CLK_ERR 0x27
#define  ME_HFS2_STATE_BUP_TEMP_DIS 0x28
#define  ME_HFS2_STATE_BUP_M0_KERN_LOAD 0x32
/*       Policy Module State */
#define  ME_HFS2_STATE_POLICY_ENTRY 0
#define  ME_HFS2_STATE_POLICY_RCVD_S3 3
#define  ME_HFS2_STATE_POLICY_RCVD_S4 4
#define  ME_HFS2_STATE_POLICY_RCVD_S5 5
#define  ME_HFS2_STATE_POLICY_RCVD_UPD 6
#define  ME_HFS2_STATE_POLICY_RCVD_PCR 7
#define  ME_HFS2_STATE_POLICY_RCVD_NPCR 8
#define  ME_HFS2_STATE_POLICY_RCVD_HOST_WAKE 9
#define  ME_HFS2_STATE_POLICY_RCVD_AC_DC 0xa
#define  ME_HFS2_STATE_POLICY_RCVD_DID 0xb
#define  ME_HFS2_STATE_POLICY_VSCC_NOT_FOUND 0xc
#define  ME_HFS2_STATE_POLICY_VSCC_INVALID 0xd
#define  ME_HFS2_STATE_POLICY_FPB_ERR 0xe
#define  ME_HFS2_STATE_POLICY_DESCRIPTOR_ERR 0xf
#define  ME_HFS2_STATE_POLICY_VSCC_NO_MATCH 0x10
/* Current PM Event Values */
#define  ME_HFS2_PMEVENT_CLEAN_MOFF_MX_WAKE 0
#define  ME_HFS2_PMEVENT_MOFF_MX_WAKE_ERROR 1
#define  ME_HFS2_PMEVENT_CLEAN_GLOBAL_RESET 2
#define  ME_HFS2_PMEVENT_CLEAN_GLOBAL_RESET_ERROR 3
#define  ME_HFS2_PMEVENT_CLEAN_ME_RESET 4
#define  ME_HFS2_PMEVENT_ME_RESET_EXCEPTION 5
#define  ME_HFS2_PMEVENT_PSEUDO_ME_RESET 6
#define  ME_HFS2_PMEVENT_S0MO_SXM3 7
#define  ME_HFS2_PMEVENT_SXM3_S0M0 8
#define  ME_HFS2_PMEVENT_NON_PWR_CYCLE_RESET 9
#define  ME_HFS2_PMEVENT_PWR_CYCLE_RESET_M3 0xa
#define  ME_HFS2_PMEVENT_PWR_CYCLE_RESET_MOFF 0xb
#define  ME_HFS2_PMEVENT_SXMX_SXMOFF 0xc

struct me_hfs2 {
	u32 bist_in_progress: 1;
	u32 reserved1: 2;
	u32 invoke_mebx: 1;
	u32 cpu_replaced_sts: 1;
	u32 mbp_rdy: 1;
	u32 mfs_failure: 1;
	u32 warm_reset_request: 1;
	u32 cpu_replaced_valid: 1;
	u32 reserved2: 4;
	u32 mbp_cleared: 1;
	u32 reserved3: 2;
	u32 current_state: 8;
	u32 current_pmevent: 4;
	u32 progress_code: 4;
} __attribute__ ((packed));

#define PCI_ME_H_GS2		0x70
#define   PCI_ME_MBP_GIVE_UP	0x01

#define PCI_ME_H_GS3		0x74

#define PCI_ME_HERES		0xbc
#define  PCI_ME_EXT_SHA1	0x00
#define  PCI_ME_EXT_SHA256	0x02
#define PCI_ME_HER(x)		(0xc0+(4*(x)))

struct me_heres {
	u32 extend_reg_algorithm: 4;
	u32 reserved: 26;
	u32 extend_feature_present: 1;
	u32 extend_reg_valid: 1;
} __attribute__ ((packed));

/*
 * Management Engine MEI registers
 */

#define MEI_H_CB_WW		0x00
#define MEI_H_CSR		0x04
#define MEI_ME_CB_RW		0x08
#define MEI_ME_CSR_HA		0x0c

struct mei_csr {
	u32 interrupt_enable: 1;
	u32 interrupt_status: 1;
	u32 interrupt_generate: 1;
	u32 ready: 1;
	u32 reset: 1;
	u32 reserved: 3;
	u32 buffer_read_ptr: 8;
	u32 buffer_write_ptr: 8;
	u32 buffer_depth: 8;
} __attribute__ ((packed));

#define MEI_ADDRESS_CORE	0x01
#define MEI_ADDRESS_AMT		0x02
#define MEI_ADDRESS_RESERVED	0x03
#define MEI_ADDRESS_WDT		0x04
#define MEI_ADDRESS_MKHI	0x07
#define MEI_ADDRESS_ICC		0x08
#define MEI_ADDRESS_THERMAL	0x09

#define MEI_HOST_ADDRESS	0

struct mei_header {
	u32 client_address: 8;
	u32 host_address: 8;
	u32 length: 9;
	u32 reserved: 6;
	u32 is_complete: 1;
} __attribute__ ((packed));

#define MKHI_GROUP_ID_CBM	0x00
#define MKHI_GROUP_ID_FWCAPS	0x03
#define MKHI_GROUP_ID_MDES	0x08
#define MKHI_GROUP_ID_GEN	0xff

#define MKHI_GLOBAL_RESET	0x0b

#define MKHI_FWCAPS_GET_RULE	0x02

#define MKHI_MDES_ENABLE	0x09

#define MKHI_GET_FW_VERSION	0x02
#define MKHI_END_OF_POST	0x0c
#define MKHI_FEATURE_OVERRIDE	0x14

struct mkhi_header {
	u32 group_id: 8;
	u32 command: 7;
	u32 is_response: 1;
	u32 reserved: 8;
	u32 result: 8;
} __attribute__ ((packed));

struct me_fw_version {
	u16 code_minor;
	u16 code_major;
	u16 code_build_number;
	u16 code_hot_fix;
	u16 recovery_minor;
	u16 recovery_major;
	u16 recovery_build_number;
	u16 recovery_hot_fix;
} __attribute__ ((packed));


#define HECI_EOP_STATUS_SUCCESS       0x0
#define HECI_EOP_PERFORM_GLOBAL_RESET 0x1

#define CBM_RR_GLOBAL_RESET	0x01

#define GLOBAL_RESET_BIOS_MRC	0x01
#define GLOBAL_RESET_BIOS_POST	0x02
#define GLOBAL_RESET_MEBX	0x03

struct me_global_reset {
	u8 request_origin;
	u8 reset_type;
} __attribute__ ((packed));

typedef enum {
	ME_NORMAL_BIOS_PATH,
	ME_S3WAKE_BIOS_PATH,
	ME_ERROR_BIOS_PATH,
	ME_RECOVERY_BIOS_PATH,
	ME_DISABLE_BIOS_PATH,
	ME_FIRMWARE_UPDATE_BIOS_PATH,
} me_bios_path;

/* Defined in me_status.c for both romstage and ramstage */
void intel_me_status(struct me_hfs *hfs, struct me_hfs2 *hfs2);

#ifdef __PRE_RAM__
void intel_early_me_status(void);
int intel_early_me_init(void);
int intel_early_me_uma_size(void);
int intel_early_me_init_done(u8 status);
#endif

#ifdef __SMM__
void intel_me_finalize_smm(void);
void intel_me8_finalize_smm(void);
#endif

/*
 * ME to BIOS Payload Datastructures and definitions. The ordering of the
 * structures follows the ordering in the ME9 BWG.
 */

#define MBP_APPID_KERNEL 1
#define MBP_APPID_INTEL_AT 3
#define MBP_APPID_HWA 4
#define MBP_APPID_ICC 5
/* Kernel items: */
#define MBP_KERNEL_FW_VER_ITEM 1
#define MBP_KERNEL_FW_CAP_ITEM 2
#define MBP_KERNEL_ROM_BIST_ITEM 3
#define MBP_KERNEL_PLAT_KEY_ITEM 4
#define MBP_KERNEL_FW_TYPE_ITEM 5
#define MBP_KERNEL_MFS_FAILURE_ITEM 6
/* Intel AT items: */
#define MBP_INTEL_AT_STATE_ITEM 1
/* ICC Items: */
#define MBP_ICC_PROFILE_ITEM 1
/* HWA Items: */
#define MBP_HWA_REQUEST_ITEM 1

#define MBP_MAKE_IDENT(appid, item) ((appid << 8) | item)
#define MBP_IDENT(appid, item) \
	MBP_MAKE_IDENT(MBP_APPID_##appid, MBP_##appid##_##item##_ITEM)

typedef  struct {
	u32  mbp_size	 : 8;
	u32  num_entries : 8;
	u32  rsvd      	 : 16;
} __attribute__ ((packed)) mbp_header;

typedef struct {
	u32  app_id  : 8;
	u32  item_id : 8;
	u32  length  : 8;
	u32  rsvd    : 8;
}  __attribute__ ((packed)) mbp_item_header;

typedef struct {
	u32       major_version  : 16;
	u32       minor_version  : 16;
	u32       hotfix_version : 16;
	u32       build_version  : 16;
} __attribute__ ((packed)) mbp_fw_version_name;

typedef struct {
	u32  full_net		: 1;
	u32  std_net		: 1;
	u32  manageability	: 1;
	u32  reserved_2		: 2;
	u32  intel_at		: 1;
	u32  intel_cls		: 1;
	u32  reserved		: 3;
	u32  intel_mpc		: 1;
	u32  icc_over_clocking	: 1;
	u32  pavp		: 1;
	u32  reserved_1		: 4;
	u32  ipv6		: 1;
	u32  kvm		: 1;
	u32  och		: 1;
	u32  vlan		: 1;
	u32  tls		: 1;
	u32  reserved_4		: 1;
	u32  wlan		: 1;
	u32  reserved_5		: 8;
} __attribute__ ((packed)) mefwcaps_sku;

typedef struct {
	mefwcaps_sku fw_capabilities;
	u8      available;
} mbp_fw_caps;

typedef struct {
	u16        device_id;
	u16        fuse_test_flags;
	u32        umchid[4];
}  __attribute__ ((packed)) mbp_rom_bist_data;

typedef struct {
	u32        key[8];
} mbp_platform_key;

typedef struct {
	u32 mobile:		1;
	u32 desktop:		1;
	u32 server:		1;
	u32 workstation:	1;
	u32 corporate:		1;
	u32 consumer:		1;
	u32 regular_super_sku:	1;
	u32 rsvd:		1;
	u32 image_type:		4;
	u32 brand:		4;
	u32 rsvd1:		16;
}  __attribute__ ((packed)) mbp_me_firmware_type;

typedef struct {
	mbp_me_firmware_type rule_data;
	u8                   available;
} mbp_plat_type;

typedef struct {
	u8        num_icc_profiles;
	u8        icc_profile_soft_strap;
	u8        icc_profile_index;
	u8        reserved;
	u32       register_lock_mask[3];
} __attribute__ ((packed)) mbp_icc_profile;

typedef struct {
	u16  lock_state		     : 1;
	u16  authenticate_module     : 1;
	u16  s3authentication  	     : 1;
	u16  flash_wear_out          : 1;
	u16  flash_variable_security : 1;
	u16  reserved		     : 11;
} __attribute__ ((packed)) tdt_state_flag;

typedef struct {
	u8           state;
	u8           last_theft_trigger;
	tdt_state_flag  flags;
}  __attribute__ ((packed)) tdt_state_info;

typedef struct {
	mbp_fw_version_name  fw_version_name;
	mbp_fw_caps	     fw_caps_sku;
	mbp_rom_bist_data    rom_bist_data;
	mbp_platform_key     platform_key;
	mbp_plat_type        fw_plat_type;
	mbp_icc_profile	     icc_profile;
	tdt_state_info	     at_state;
	u32		     mfsintegrity;
} me_bios_payload;

struct me_fwcaps {
	u32 id;
	u8 length;
	mefwcaps_sku caps_sku;
	u8 reserved[3];
} __attribute__ ((packed));

#endif /* _INTEL_ME_H */

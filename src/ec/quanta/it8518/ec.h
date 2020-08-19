/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * EC communication interface for QUANTA IT8518 Embedded Controller.
 */

#ifndef _EC_QUANTA_IT8518_EC_H
#define _EC_QUANTA_IT8518_EC_H

#define EC_IO 0x100 /* Mainboard specific. Could be Kconfig option */
#define EC_IO_HIGH EC_IO + 1
#define EC_IO_LOW  EC_IO + 2
#define EC_IO_DATA EC_IO + 3

/* Wait 400ms for keyboard controller answers */
#define KBC_TIMEOUT_IN_MS           400

// 60h/64h Command Interface
#define KBD_DATA                    0x60
#define KBD_COMMAND                 0x64
#define KBD_STATUS                  0x64
#define   KBD_IBF                   (1 << 1) // 1: input buffer full (data ready for ec)
#define   KBD_OBF                   (1 << 0) // 1: output buffer full (data ready for host)

#define EC_KBD_SMI_EVENT            0xCD
#define EC_KBD_CMD_UNMUTE           0xE8
#define EC_KBD_CMD_MUTE             0xE9

u8 ec_kbc_read_ob(void);
void ec_kbc_write_cmd(u8 cmd);
void ec_kbc_write_ib(u8 data);

// 62h/66h Command Interface
#define EC_DATA        0x62
#define EC_SC          0x66	// Status & Control Register
#define   SMI_EVT      (1 << 6)	// 1: SMI event was triggered
#define   SCI_EVT      (1 << 5) // 1: SCI event was triggered

// EC Commands (defined in ec_function_spec v3.12)
#define RD_EC          0x80
#define WR_EC          0x81
#define QR_EC          0x84

#define EC_CMD_EXIT_BOOT_BLOCK      0x85
#define EC_CMD_NOTIFY_ACPI_ENTER    0x86
#define EC_CMD_NOTIFY_ACPI_EXIT     0x87
#define EC_CMD_WARM_RESET           0x8C

// ECRAM Offsets
#define EC_PERIPH_CNTL_3            0x0D
#define EC_USB_S3_EN                0x26
#define EC_PERIPH_STAT_3            0x35
#define EC_THERM_0                  0x78
#define EC_WAKE_SRC_ENABLE          0xBF
#define EC_FW_VER                   0xE8 // 2 Bytes
#define EC_IF_MIN_VER               0xEB
#define EC_STATUS_REG               0xEC
#define EC_IF_MAJ_VER               0xEF
#define EC_MBAT_STATUS              0x0138

// EC 0.83b added status bits:
//  BIT0=EC in RO mode
//  BIT1=Recovery Key Sequence Detected
#define EC_IN_RO_MODE               0x1
#define EC_IN_RECOVERY_MODE         0x3

// EC 0.86a added enable bit:
#define EC_LID_WAKE_ENABLE          0x4

u8 ec_read_ob(void);
void ec_write_cmd(u8 cmd);
void ec_write_ib(u8 data);

u8 ec_read(u16 addr);
void ec_write(u16 addr, u8 data);
u8 ec_it8518_get_event(void);
void ec_it8518_enable_wake_events(void);

#endif /* _EC_QUANTA_IT8518_EC_H */

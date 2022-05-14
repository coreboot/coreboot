/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _EC_ACPI_H
#define _EC_ACPI_H

#include <types.h>

#define EC_DATA	0x62
#define EC_SC	0x66

/* EC_SC input */
#define   EC_SMI_EVT	(1 << 6) // 1: SMI event pending
#define   EC_SCI_EVT	(1 << 5) // 1: SCI event pending
#define   EC_BURST	(1 << 4) // 1: controller is in burst mode
#define   EC_CMD	(1 << 3) // 1: byte in data register is command
				 // 0: byte in data register is data
#define   EC_IBF	(1 << 1) // 1: input buffer full (data ready for ec)
#define   EC_OBF	(1 << 0) // 1: output buffer full (data ready for host)
/* EC_SC output */
#define   RD_EC		0x80 // Read Embedded Controller
#define   WR_EC		0x81 // Write Embedded Controller
#define   BE_EC		0x82 // Burst Enable Embedded Controller
#define   BD_EC		0x83 // Burst Disable Embedded Controller
#define   QR_EC		0x84 // Query Embedded Controller

bool ec_ready_send(int timeout_us);
bool ec_ready_recv(int timeout_us);
int send_ec_command(u8 command);
int send_ec_command_timeout(u8 command, int timeout);
int send_ec_data(u8 data);
int send_ec_data_timeout(u8 data, int timeout);
int recv_ec_data(void);
int recv_ec_data_timeout(int timeout);
void ec_clear_out_queue(void);
u8 ec_status(void);
u8 ec_query(void);
u8 ec_read(u8 addr);
int ec_write(u8 addr, u8 data);
void ec_set_bit(u8 addr, u8 bit);
void ec_clr_bit(u8 addr, u8 bit);
void ec_set_ports(u16 cmd_reg, u16 data_reg);

#endif /* _EC_ACPI_H */

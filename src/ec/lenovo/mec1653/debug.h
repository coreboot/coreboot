/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_LENOVO_MEC1653_DEBUG_H
#define EC_LENOVO_MEC1653_DEBUG_H

// The following location (via either EC0 or EC1) can be used to interact with the debug interface
#define EC_DEBUG_CMD		0x3d

// RW unlock key index
#define DEBUG_RW_KEY_IDX	1

#define EC_SEND_TIMEOUT_US	20000	// 20ms
#define EC_RECV_TIMEOUT_US	320000	// 320ms

#define EC0_CMD		0x0066
#define EC0_DATA	0x0062
#define EC1_CMD		0x1604
#define EC1_DATA	0x1600
#define EC2_CMD		0x1634
#define EC2_DATA	0x1630
#define EC3_CMD		0x161c
#define EC3_DATA	0x1618


// Read loaded debug key mask
uint16_t debug_loaded_keys(void);

void debug_read_key(uint8_t i, uint8_t *key);

void debug_write_key(uint8_t i, const char *hex_key);

uint32_t debug_read_dword(uint32_t addr);

void debug_write_dword(uint32_t addr, uint32_t val);

#endif /* EC_LENOVO_MEC1653_DEBUG_H */

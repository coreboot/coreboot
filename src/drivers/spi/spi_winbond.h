/* SPDX-License-Identifier: GPL-2.0-only */

/* Winbond specific function */
/* M25Pxx-specific commands */
#define CMD_W25_WREN		0x06 /* Write Enable */
#define CMD_W25_WRDI		0x04 /* Write Disable */
#define CMD_W25_RDSR		0x05 /* Read Status Register */
#define CMD_W25_WRSR		0x01 /* Write Status Register */
#define CMD_W25_RDSR2		0x35 /* Read Status2 Register */
#define CMD_W25_WRSR2		0x31 /* Write Status2 Register */
#define CMD_W25_READ		0x03 /* Read Data Bytes */
#define CMD_W25_FAST_READ	0x0b /* Read Data Bytes at Higher Speed */
#define CMD_W25_PP		0x02 /* Page Program */
#define CMD_W25_SE		0x20 /* Sector (4K) Erase */
#define CMD_W25_RDID		0x9f /* Read ID */
#define CMD_W25_BE		0xd8 /* Block (64K) Erase */
#define CMD_W25_CE		0xc7 /* Chip Erase */
#define CMD_W25_DP		0xb9 /* Deep Power-down */
#define CMD_W25_RES		0xab /* Release from DP and Read Signature */
#define CMD_VOLATILE_SREG_WREN	0x50 /* Write Enable for Volatile SREG */

/* tw: Maximum time to write a flash cell in milliseconds */
#define WINBOND_FLASH_TIMEOUT 30
